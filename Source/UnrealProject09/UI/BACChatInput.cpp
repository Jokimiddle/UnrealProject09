#include "UI/BACChatInput.h"
#include "Components/EditableTextBox.h"
#include "Player/BACPlayerController.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UBACChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UBACChatInput::NativeDestruct()
{
	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UBACChatInput::UpdateNotificationText(const FString& InChatMessageString)
{
	if (IsValid(NotificationText) == false) return;
	if (IsValid(Notification_Fadeout) == false) return;

	NotificationText->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (IsAnimationPlaying(Notification_Fadeout) == true)
	{
		StopAnimation(Notification_Fadeout);
	}

	NotificationText->SetText(FText::FromString(InChatMessageString));
	PlayAnimation(Notification_Fadeout);
}

void UBACChatInput::UpdateTimerValue(const FString& InTime)
{
	if (IsValid(TimerValue) == false) return;
	FText Time = FText::FromString(InTime);
	TimerValue->SetText(Time);
}

void UBACChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* PlayerController = GetOwningPlayer();
		if (IsValid(PlayerController) == true)
		{
			ABACPlayerController* BACPlayerController = Cast<ABACPlayerController>(PlayerController);
			if (IsValid(BACPlayerController) == true)
			{
				BACPlayerController->SetChatMessageString(Text.ToString());

				EditableTextBox_ChatInput->SetText(FText());		// 텍스트 비우기, 빈 텍스트로 교체
			}
		}
	}
}

