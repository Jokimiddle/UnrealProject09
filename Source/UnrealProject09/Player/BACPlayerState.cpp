#include "Player/BACPlayerState.h"
#include "Net/UnrealNetwork.h"

ABACPlayerState::ABACPlayerState()
{
	PlayerNameString = TEXT("None");
	CurrentGuessCount = 0;
	MaxGuessCount = 3;
	bReplicates = true;
}

void ABACPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString); // 레플리케이션 프로퍼티 등록
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount); // 레플리케이션이 많으면 오버헤드 가능성?
}

FString ABACPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}
