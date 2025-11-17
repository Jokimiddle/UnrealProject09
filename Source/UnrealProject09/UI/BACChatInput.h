#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BACChatInput.generated.h"

class UEditableTextBox;
UCLASS()
class UNREALPROJECT09_API UBACChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
};
