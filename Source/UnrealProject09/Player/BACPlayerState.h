#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BACPlayerState.generated.h"

UCLASS()
class UNREALPROJECT09_API ABACPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated)
	FString PlayerNameString;
	
	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;
		
	ABACPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	FString GetPlayerInfoString();
};
