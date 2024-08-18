// Copyright Epic Games, Inc. All Rights Reserved.


#include "LibreFPVGameModeBase.h"
#include "EngineUtils.h"
#include "Checkpoint.h"
#include "GameStateBase2.h"
#include "PlayerState2.h"
#include "HUD2.h"
#include "Quadcopter.h"
#include "GameFramework/PlayerStart.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Net/UnrealNetwork.h"

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
			// empty array
			GameState2->Checkpoints.Empty();
			GameState2->StartingBlocks.Empty();
			// gather starting blocks
			for (TActorIterator<APlayerStart> i(World); i; ++i) {
				GameState2->StartingBlocks.Add(*i);
			}
			// gather all checkpoints, and add to array
			for (TActorIterator<ACheckpoint> i(World); i; ++i) {
				GameState2->Checkpoints.Add(*i);
			}
			GameState2->Checkpoints.Sort();
			for (TActorIterator<APlayerState2> i(World); i; ++i) {
				i->CurrentCheckpointIndex = 0;
				i->NextCheckpointIndex = 1;
			}
			MARK_PROPERTY_DIRTY_FROM_NAME(AGameStateBase2, Checkpoints, GameState2);
			MARK_PROPERTY_DIRTY_FROM_NAME(AGameStateBase2, StartingBlocks, GameState2);

			GameState2->bResetTrack = true;
			GameState2->bReceivedCheckpoints = true;
			GameState2->bReceivedStartingBlocks = true;
			GameState2->OnRep_bResetTrack();
		}
	}
}
