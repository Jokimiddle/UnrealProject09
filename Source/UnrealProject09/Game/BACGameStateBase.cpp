#include "Game/BACGameStateBase.h"
#include "Player/BACPlayerController.h"
#include "Kismet/GameplayStatics.h"

void ABACGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)	// 권한 체크. 클라이언트인 경우
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PlayerController) == true)
		{
			ABACPlayerController* BACPlayerController = Cast<ABACPlayerController>(PlayerController);
			if (IsValid(BACPlayerController) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				BACPlayerController->PrintChatMessageString(NotificationString);
			}
		}
	}
}