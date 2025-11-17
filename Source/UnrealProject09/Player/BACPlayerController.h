#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BACPlayerController.generated.h"

class UBACChatInput;
UCLASS()
class UNREALPROJECT09_API ABACPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBACChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UBACChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;

};
