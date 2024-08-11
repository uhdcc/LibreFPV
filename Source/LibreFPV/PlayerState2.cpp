// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState2.h"

APlayerState2::APlayerState2() {
	CurrentCheckpointIndex = -1;
	NextCheckpointIndex = -1;
	CheckpointSound = nullptr;

	CheckpointSound = LoadObject<USoundBase>(
		nullptr,
		TEXT("SoundWave'/Game/CheckpointSound.CheckpointSound'")
	);
}
