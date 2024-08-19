// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState2.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

APlayerState2::APlayerState2() {
	CurrentCheckpointIndex = -1;
	NextCheckpointIndex = -1;
	CheckpointSound = nullptr;
	PersonalBest = 0.f;

	CheckpointSound = LoadObject<USoundBase>(
		nullptr,
		TEXT("SoundWave'/Game/CheckpointSound.CheckpointSound'")
	);
}
void APlayerState2::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;

	//SharedParams.bIsPushBased = true;
	SharedParams.bIsPushBased = false;
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerState2, CheckpointSplits, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerState2, PersonalBest, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerState2, PersonalBestSplits, SharedParams);

}
