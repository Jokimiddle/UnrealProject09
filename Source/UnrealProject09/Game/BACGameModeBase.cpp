#include "Game/BACGameModeBase.h"
#include "Game/BACGameStateBase.h"
#include "Player/BACPlayerController.h"
#include "Player/BACPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "UnrealProject09.h"
#include "Containers/UnrealString.h"

ABACGameModeBase::ABACGameModeBase()
{
	TimerTime = 30;
	CurrentTimerTime = TimerTime;
}

void ABACGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABACPlayerController* BACPlayerController = Cast<ABACPlayerController>(NewPlayer);
	if (IsValid(BACPlayerController) == true)
	{
		//BACPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(BACPlayerController); // 플레이어 배열에 저장

		ABACPlayerState* BACPlayerState = BACPlayerController->GetPlayerState<ABACPlayerState>();
		if (IsValid(BACPlayerState) == true)
		{
			BACPlayerState->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num()); // 플레이어 식별을 위한 이름 지정
		}

		ABACGameStateBase* BACGameStateBase = GetGameState<ABACGameStateBase>();
		if (IsValid(BACGameStateBase) == true)
		{
			BACGameStateBase->MulticastRPCBroadcastLoginMessage(BACPlayerState->PlayerNameString); // 플레이어 이름 전달
		}
	}
}

FString ABACGameModeBase::GenerateSecretNumber() // 랜덤 3자리 숫자 생성
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());	// Tick을 이용한 랜덤 값
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	UE_LOG(LogTemp, Display, TEXT("Created Secret Number: %s"), *Result);
	return Result;
}

bool ABACGameModeBase::IsGuessNumberString(const FString& InNumberString) // 수 유효성 체크
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3) // 입력값 자리수 체크
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0') // 숫자가 아닌 문자 형태인지 체크
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false) // 중복된 값 체크
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ABACGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i]) // 입력된 숫자 중에서 SecretNumber에 포함되고 자리가 같은 경우
		{
			StrikeCount++; // Strike
		}
		else // 동일한 자리가 아닌건 확실할 때
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar)) // 입력된 숫자 중에서 SecretNumber에 포함되는 경우
			{
				BallCount++; // Ball
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0) // 아무것도 해당되지 않는 경우
	{
		return TEXT("OUT"); // Out
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount); // Strike:Ball Print
}

bool ABACGameModeBase::IncreaseGuessCount(ABACPlayerController* InChattingPlayerController)
{
	ABACPlayerState* BACPlayerState = InChattingPlayerController->GetPlayerState<ABACPlayerState>();
	if (IsValid(BACPlayerState) == true)
	{
		if (BACPlayerState->CurrentGuessCount < BACPlayerState->MaxGuessCount)
		{
			BACPlayerState->CurrentGuessCount++;
			return true;
		}
	}
	return false;
}

void ABACGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SecretNumberString.Empty();
}

void ABACGameModeBase::PrintChatMessageString(ABACPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	if (InChatMessageString.IsEmpty()) return; // 빈 문자면 리턴

	FString ChatMessageString = InChatMessageString; // 대화 메시지 or 게임 메시지 체크
	FString DelimiterString = TEXT(": "); // 클라이언트 정보를 제외한 순수 메시지 정제 목적
	int32 Index = ChatMessageString.Find(DelimiterString, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	Index += DelimiterString.Len();
	FString RefinedString = ChatMessageString.RightChop(Index);
	if (IsGuessNumberString(RefinedString) == true && SecretNumberString.IsEmpty() == false) // 유효한, 포맷 문자인지 체크
	{
		if (IncreaseGuessCount(InChattingPlayerController) == false) // 게임 기회 소모 및 소모가 가능 불가능 한 경우
		{
			ABACPlayerState* BACPlayerState = InChattingPlayerController->GetPlayerState<ABACPlayerState>();
			if (IsValid(BACPlayerState) == false) return;

			int32 MaxGuessCount = BACPlayerState->MaxGuessCount;
			FString MessageString = TEXT("You have used all " + FString::FromInt(MaxGuessCount) + " attempts.");
			InChattingPlayerController->ClientRPCPrintSystemMessageString(MessageString, FColor::Red);
			return;
		}

		FString JudgeResultString = JudgeResult(SecretNumberString, RefinedString); // 숫자 야구 체크

		ABACPlayerState* BACPlayerState = InChattingPlayerController->GetPlayerState<ABACPlayerState>();
		DelimiterString = TEXT("/"); // 클라이언트 플레이어의 기회 소모를 반영하기 위함
		Index = ChatMessageString.Find(DelimiterString, ESearchCase::CaseSensitive, ESearchDir::FromStart);
		Index -= 1;
		ChatMessageString[Index] = BACPlayerState->CurrentGuessCount + TEXT('0'); // IncreaseGuessCount() 증가값 반영

		for (TActorIterator<ABACPlayerController> It(GetWorld()); It; ++It)
		{
			ABACPlayerController* BACPlayerController = *It;
			if (IsValid(BACPlayerController) == true)
			{
				FString CombinedMessageString = ChatMessageString + TEXT(" -> ") + JudgeResultString;
				BACPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString); // 결과 반환

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else // 일반 대화 메시지인 경우 or 게임 시작 메시지
	{
		if (InChattingPlayerController == AllPlayerControllers[0])	// 메시지 송신자가 방장인 경우
		{
			if (RefinedString[0] == '/') // 명령어 태그 확인
			{
				if (RefinedString.Equals("/start")) // 게임 시작 명령어
				{
					if (SecretNumberString.IsEmpty() == false) // 실패:게임이 아직 실행중
					{
						FString FailMessage = TEXT("Fail: Game is Running");
						InChattingPlayerController->ClientRPCPrintChatMessageString(FailMessage);
						return;
					}
					// 성공:게임 시작
					StartGame();
					for (const auto& BACPlayerController : AllPlayerControllers)
					{
						FString GameStartMessage = TEXT("Game is Started!");
						BACPlayerController->ClientRPCSetNotificationTextMessage(GameStartMessage);
					}
					return;
				}
				else if (RefinedString.Equals("/restart")) // 게임 재시작 명령어
				{
					if (SecretNumberString.IsEmpty() == false) // 게임이 아직 동작 중
					{
						ResetGame();
					}
					StartGame();
					for (const auto& BACPlayerController : AllPlayerControllers)
					{
						FString GameStartMessage = TEXT("Game is Started!");
						BACPlayerController->ClientRPCSetNotificationTextMessage(GameStartMessage);
					}
					return;
				}
				FString InvalidCommandMessage = TEXT("Invalid Command");
				InChattingPlayerController->ClientRPCPrintChatMessageString(InvalidCommandMessage);
				return;
			}
		}
		for (TActorIterator<ABACPlayerController> It(GetWorld()); It; ++It)
		{
			ABACPlayerController* BACPlayerController = *It;
			if (IsValid(BACPlayerController) == true)
			{
				BACPlayerController->ClientRPCPrintChatMessageString(ChatMessageString); // 일반 채팅
			}
		}
	}
}

void ABACGameModeBase::StartGame()
{
	UE_LOG(LogTemp, Display, TEXT("[GameMode] StartGame"));
	SecretNumberString = GenerateSecretNumber();
	CurrentTimerTime = TimerTime;
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &ABACGameModeBase::TimerTick, 1.0, true);
	UE_LOG(LogTemp, Display, TEXT("[GameMode] GameTimer Set Successfully"));
	UE_LOG(LogTemp, Display, TEXT("[GameMode] Game Started Successfully"));
}

void ABACGameModeBase::ResetGame()
{
	SecretNumberString.Empty();
	GetWorldTimerManager().ClearTimer(GameTimerHandle);
	CurrentTimerTime = 0;
	UE_LOG(LogTemp, Display, TEXT("[GameMode] ResetGame"));
	for (const auto& BACPlayerController : AllPlayerControllers) // 클라이언트 플레이어 컨트롤러 순회
	{
		ABACPlayerState* BACPlayerState = BACPlayerController->GetPlayerState<ABACPlayerState>();
		if (IsValid(BACPlayerState) == true)
		{
			BACPlayerState->CurrentGuessCount = 0; // 게임 기회 리셋
		}
	}
	UE_LOG(LogTemp, Display, TEXT("[GameMode] Game Reseted Successfully"));
}

void ABACGameModeBase::JudgeGame(ABACPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount) // 본인이 우승
	{
		ABACPlayerState* BACPlayerState = InChattingPlayerController->GetPlayerState<ABACPlayerState>();
		for (const auto& BACPlayerController : AllPlayerControllers) // 클라이언트 플레이어 컨트롤러 순회
		{
			if (IsValid(BACPlayerState) == true)
			{
				FString CombinedMessageString = BACPlayerState->PlayerNameString + TEXT(" has won the game."); // 플레이어 이름
				//BACPlayerController->NotificationText = FText::FromString(CombinedMessageString);
				BACPlayerController->ClientRPCSetNotificationTextMessage(CombinedMessageString);
				ResetGame();
			}
		}
	}
	else // 다른 플레이어가 우승 or 무승부
	{
		bool bIsDraw = true;
		for (const auto& BACPlayerController : AllPlayerControllers) // 클라이언트 플레이어 컨트롤러 순회
		{
			ABACPlayerState* BACPlayerState = BACPlayerController->GetPlayerState<ABACPlayerState>();
			if (IsValid(BACPlayerState) == true)
			{
				if (BACPlayerState->CurrentGuessCount < BACPlayerState->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw) // 무승부
		{
			for (const auto& BACPlayerController : AllPlayerControllers)
			{
				//BACPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));
				FString DrawMessage = TEXT("Draw...");
				BACPlayerController->ClientRPCSetNotificationTextMessage(DrawMessage);
			}
			ResetGame();
		}
	}
}

void ABACGameModeBase::TimerTick()
{
	if (CurrentTimerTime > 0)
	{
		FString Time = FString::SanitizeFloat(CurrentTimerTime, 0);
		for (const auto& BACPlayerController : AllPlayerControllers)
		{
			BACPlayerController->ClientRPCSetTimerMessage(Time);
		}
		CurrentTimerTime -= 1.0f;
	}
	else
	{
		TimeOut();
		FString ZeroTime = TEXT("0");
		for (const auto& BACPlayerController : AllPlayerControllers)
		{
			BACPlayerController->ClientRPCSetTimerMessage(ZeroTime);
		}
	}
}

void ABACGameModeBase::TimeOut()
{
	GetWorldTimerManager().ClearTimer(GameTimerHandle);
	for (const auto& BACPlayerController : AllPlayerControllers)
	{
		FString DrawMessage = TEXT("Time Out Draw...");
		BACPlayerController->ClientRPCSetNotificationTextMessage(DrawMessage);
	}
	ResetGame();
}

