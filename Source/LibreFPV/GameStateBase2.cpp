// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateBase2.h"
#include "EngineUtils.h"
#include "Checkpoint.h"
#include "HUD2.h"
#include "PlayerState2.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Quadcopter.h"
#include "GameFramework/PlayerStart.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Templates\SharedPointer.h"
#include "Components/BoxComponent.h"

AGameStateBase2::AGameStateBase2() {
	TotalNumberOfCheckpoints = 0;
	bResetTrack = false;
	bReceivedCheckpoints = false;
	bReceivedStartingBlocks = false;
}
void AGameStateBase2::BeginPlay() {
	Super::BeginPlay();
}
void AGameStateBase2::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	// if this is a valid player...
	if (OtherActor && OtherActor->GetInstigatorController() && OtherActor->GetInstigatorController()->IsLocalPlayerController()) {
		if (auto PlayerState = Cast<APlayerState2>(Cast<APlayerController>(OtherActor->GetInstigatorController())->PlayerState)) {
			if (auto Checkpoint = Cast<ACheckpoint>(OverlappedComponent->GetOwner())) {
				auto CheckpointIndex = Checkpoint->GetCheckpointIndex();
				// if this is the player's next checkpoint to trigger...
				if (CheckpointIndex == PlayerState->CurrentCheckpointIndex) {
					if (FVector::DotProduct(Checkpoint->CheckpointTrigger->GetForwardVector(), OtherActor->GetVelocity().GetSafeNormal()) > 0.f) {
						auto SplitTime = 0.f;
						auto SplitDifference = 0.f;
						// first checkpoint
						if (CheckpointIndex == 0) {
							PlayerState->RunStartTime = FPlatformTime::Seconds();
						}
						else {
							SplitTime = FPlatformTime::Seconds() - PlayerState->RunStartTime;
							// update playerstate
							if (PlayerState->CheckpointSplits.IsValidIndex(CheckpointIndex - 1)) {
								PlayerState->CheckpointSplits[CheckpointIndex - 1] = SplitTime;
							}
							else {
								PlayerState->CheckpointSplits.Add(SplitTime);
							}
							SplitDifference = (PlayerState->PersonalBest == 0.f) ? -SplitTime : (SplitTime - PlayerState->PersonalBestSplits[CheckpointIndex - 1]);
							// last checkpoint
							if (CheckpointIndex == (TotalNumberOfCheckpoints - 1)) {
								if (PlayerState->PersonalBest == 0.f || SplitTime < PlayerState->PersonalBest) {
									PlayerState->PersonalBest = SplitTime;
									PlayerState->PersonalBestSplits = PlayerState->CheckpointSplits;
								}
							}
						}
						// play sound
						UGameplayStatics::PlaySound2D(PlayerState, PlayerState->CheckpointSound);
						// increment indexes
						PlayerState->CurrentCheckpointIndex = PlayerState->NextCheckpointIndex;
						PlayerState->NextCheckpointIndex = (PlayerState->NextCheckpointIndex + 1) % TotalNumberOfCheckpoints;
						// update player hud
						if (auto HUD2 = Cast<APlayerController>(OtherActor->GetInstigatorController())->GetHUD<AHUD2>()) {
							HUD2->CurrentCheckpoint = FWaypoint(*Checkpoints[PlayerState->CurrentCheckpointIndex]);
							HUD2->NextCheckpoint = FWaypoint(*Checkpoints[PlayerState->NextCheckpointIndex]);
							if (CheckpointIndex > 0) HUD2->CheckpointSplit->UpdateCheckpointSplit(SplitDifference);
						}
					}
				}
			}
		}
	}
}

void AGameStateBase2::RestartRun(AQuadcopter* Quadcopter) {
	if (auto PlayerState = Cast<APlayerState2>(Cast<APlayerController>(Quadcopter->GetInstigatorController())->PlayerState)) {
		// set starting location and rotation
		auto InitialLocation = FVector::ZeroVector;
		auto InitialRotation = FRotator::ZeroRotator;
		if (StartingBlocks.IsValidIndex(0)) {
			InitialLocation = StartingBlocks[0]->GetActorLocation();
			InitialRotation = StartingBlocks[0]->GetActorRotation();
		}
		// move quadcopter and stop physics
		Quadcopter->SetActorLocationAndRotation(InitialLocation, InitialRotation);
		Quadcopter->QuadcopterCollision->SetPhysicsLinearVelocity(FVector::ZeroVector);
		Quadcopter->QuadcopterCollision->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		// initialize player state
		PlayerState->CurrentCheckpointIndex = 0;
		PlayerState->NextCheckpointIndex = 1;
		PlayerState->RunStartTime = FPlatformTime::Seconds();
		// initialize hud
		if (auto HUD2 = Cast<APlayerController>(Quadcopter->GetInstigatorController())->GetHUD<AHUD2>()) {
			HUD2->CurrentCheckpoint = FWaypoint(*Checkpoints[PlayerState->CurrentCheckpointIndex]);
			HUD2->NextCheckpoint = FWaypoint(*Checkpoints[PlayerState->NextCheckpointIndex]);
		}
	}
}
void AGameStateBase2::OnRep_bResetTrack() {
	if (bResetTrack && bReceivedCheckpoints && bReceivedStartingBlocks) {
		bResetTrack = false;
		bReceivedCheckpoints = false;
		bReceivedStartingBlocks = false;
		if (auto World = GetWorld()) {
			// if there's more than 1 checkpoint...
			TotalNumberOfCheckpoints = Checkpoints.Num();
			if (TotalNumberOfCheckpoints > 1) {
				// bind to checkpoints
				for (auto i : Checkpoints) {
					i->CheckpointTrigger->OnComponentBeginOverlap.AddDynamic(this, &AGameStateBase2::OnOverlap);
				}
				// initialize huds
				for (TActorIterator<AHUD2> i(World); i; ++i) {
					// todo : some of this stuff is redundant now that i made RestartRun()
					i->CurrentCheckpoint = FWaypoint(*Checkpoints[0]);
					i->NextCheckpoint = FWaypoint(*Checkpoints[1]);
					// initialize quadcopters
					if (auto Quadcopter = Cast<AQuadcopter>(i->GetOwningPawn())) {
						i->bShowCheckpointMarkers = true;
						Quadcopter->WantsRestartRun.AddDynamic(this, &AGameStateBase2::RestartRun);
						RestartRun(Quadcopter);
					}
				}
			}
		}
	}
}
void AGameStateBase2::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	
	SharedParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(AGameStateBase2, Checkpoints, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(AGameStateBase2, StartingBlocks, SharedParams);
	SharedParams.bIsPushBased = false;
	DOREPLIFETIME_WITH_PARAMS_FAST(AGameStateBase2, bResetTrack, SharedParams);

}

void AGameStateBase2::OnRep_Checkpoints() {
	bReceivedCheckpoints = true;
	OnRep_bResetTrack();
}

void AGameStateBase2::OnRep_StartingBlocks() {
	bReceivedStartingBlocks = true;
	OnRep_bResetTrack();
}
