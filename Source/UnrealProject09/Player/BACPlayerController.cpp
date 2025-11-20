#include "Player/BACPlayerController.h"
#include "Player/BACPlayerState.h"
#include "UI/BACChatInput.h"
#include "UnrealProject09.h"
#include "Game/BACGameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

ABACPlayerController::ABACPlayerController()
{
	bReplicates = true;
}
void ABACPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false) return;	// Local(Owning) Client only

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UBACChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
			
			FString ConnectedMessage = TEXT("Connected to the game server.");
			ClientRPCSetNotificationTextMessage(ConnectedMessage);
		}
	}
}

void ABACPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	if (IsLocalController() == true)
	{
		ABACPlayerState* BACPlayerState = GetPlayerState<ABACPlayerState>();
		if (IsValid(BACPlayerState) == true)
		{
			FString CombinedMessageString = BACPlayerState->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
			ServerRPCPrintChatMessageString(CombinedMessageString); // 본인 이름과 메시지를 전달
		}
	}
}

void ABACPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	FString NetModeString = ChatXFunctionLibrary::GetNetModeString(this);
	FString CombinedMessageString = FString::Printf(TEXT("%s: %s"), *NetModeString, *InChatMessageString);
	ChatXFunctionLibrary::MyPrintString(this, CombinedMessageString, 10.f);
}

void ABACPlayerController::PrintSystemMessageString(const FString& InChatMessageString, const FColor& Color = FColor::Silver)
{
	ChatXFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f, Color);
}

void ABACPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ABACPlayerController::ClientRPCPrintSystemMessageString_Implementation(const FString& InChatMessageString, const FColor& Color = FColor::Silver)
{
	PrintSystemMessageString(InChatMessageString, Color);
}

void ABACPlayerController::ClientRPCSetNotificationTextMessage_Implementation(const FString& InChatMessageString)
{
	ChatInputWidgetInstance->UpdateNotificationText(InChatMessageString);
}

void ABACPlayerController::ClientRPCSetTimerMessage_Implementation(const FString& InChatMessageString)
{
	ChatInputWidgetInstance->UpdateTimerValue(InChatMessageString);
}

void ABACPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	if (IsValid(GameModeBase) == true)
	{
		ABACGameModeBase* BACGameModeBase = Cast<ABACGameModeBase>(GameModeBase);
		if (IsValid(BACGameModeBase) == true)
		{
			BACGameModeBase->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

void ABACPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ThisClass, NotificationText);
}


