#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BACGameStateBase.generated.h"

UCLASS()
class UNREALPROJECT09_API ABACGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString);
};
