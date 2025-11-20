#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BACChatInput.generated.h"

class UEditableTextBox;
class UTextBlock;
class UWidgetAnimation;
UCLASS()
class UNREALPROJECT09_API UBACChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NotificationText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Notification_Fadeout;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerValue;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void UpdateNotificationText(const FString& InChatMessageString);
	UFUNCTION()
	void UpdateTimerValue(const FString& InTime);
protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
