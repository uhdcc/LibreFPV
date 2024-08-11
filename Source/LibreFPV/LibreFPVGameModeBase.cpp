// Copyright Epic Games, Inc. All Rights Reserved.


#include "LibreFPVGameModeBase.h"
#include "EngineUtils.h"
#include "Checkpoint.h"
#include "GameStateBase2.h"
#include "PlayerState2.h"
#include "HUD2.h"

ALibreFPVGameModeBase::ALibreFPVGameModeBase() {
	GameStateClass = AGameStateBase2::StaticClass();
	PlayerStateClass = APlayerState2::StaticClass();
	HUDClass = AHUD2::StaticClass();
}
void ALibreFPVGameModeBase::BeginPlay() {
	Super::BeginPlay();
	GameState2 = GetGameState<AGameStateBase2>();
	ResetTrack();
}
void ALibreFPVGameModeBase::ResetTrack() {
	if (auto World = GetWorld()) {
		if (GameState2) {
			for (TActorIterator<ACheckpoint> i(World); i; ++i) {
				GameState2->Checkpoints.Add(*i);
			}
			for (TActorIterator<APlayerState2> i(World); i; ++i) {
				i->CurrentCheckpointIndex = 0;
				i->NextCheckpointIndex = 1;
			}
			GameState2->ResetTrack();
		}
	}
}
