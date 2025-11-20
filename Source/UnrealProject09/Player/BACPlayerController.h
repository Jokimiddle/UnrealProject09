#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BACPlayerController.generated.h"

class UBACChatInput;
class UUserWidget;

UCLASS()
class UNREALPROJECT09_API ABACPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// UPROPERTY(Replicated, BlueprintReadOnly)
	// FText NotificationText;

	ABACPlayerController();
	virtual void BeginPlay() override;
	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);
	void PrintSystemMessageString(const FString& InChatMessageString, const FColor& Color);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintSystemMessageString(const FString& InChatMessageString, const FColor& Color);
	UFUNCTION(Client, Reliable)
	void ClientRPCSetNotificationTextMessage(const FString& InChatMessageString);
	UFUNCTION(Client, Reliable)
	void ClientRPCSetTimerMessage(const FString& InChatMessageString);
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBACChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UBACChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;
};
