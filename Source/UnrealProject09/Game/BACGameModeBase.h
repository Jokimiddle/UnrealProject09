#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BACGameModeBase.generated.h"

class ABACPlayerController;
UCLASS()
class UNREALPROJECT09_API ABACGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ABACGameModeBase();
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();
	bool IsGuessNumberString(const FString& InNumberString);
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	bool IncreaseGuessCount(ABACPlayerController* InChattingPlayerController);
	
	virtual void BeginPlay() override;
	void PrintChatMessageString(ABACPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void StartGame();
	void ResetGame();

	void JudgeGame(ABACPlayerController* InChattingPlayerController, int InStrikeCount);

	void TimerTick();
	void TimeOut();
protected:
	FString SecretNumberString; // 숫자야구 정답
	TArray<TObjectPtr<ABACPlayerController>> AllPlayerControllers; // 접속하는 플레이어를 저장할 배열

	FTimerHandle GameTimerHandle;
	float TimerTime;
	float CurrentTimerTime;
};
