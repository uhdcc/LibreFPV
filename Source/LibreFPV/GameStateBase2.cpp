// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateBase2.h"
#include "EngineUtils.h"
#include "Checkpoint.h"
#include "HUD2.h"
#include "PlayerState2.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AGameStateBase2::AGameStateBase2() {
	TotalNumberOfCheckpoints = 0;
}
void AGameStateBase2::BeginPlay() {
	Super::BeginPlay();
}
void AGameStateBase2::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor && OtherActor->GetInstigatorController() && OtherActor->GetInstigatorController()->IsLocalPlayerController()) {
		if (auto PlayerState = Cast<APlayerState2>(Cast<APlayerController>(OtherActor->GetInstigatorController())->PlayerState)) {
			auto CheckpointIndex = Cast<ACheckpoint>(OverlappedComponent->GetOwner())->CheckpointIndex;
			if (CheckpointIndex == PlayerState->CurrentCheckpointIndex) {				
				if (FVector::DotProduct(OverlappedComponent->GetForwardVector(), OtherActor->GetVelocity().GetSafeNormal()) > 0.f) {
					UGameplayStatics::PlaySound2D(PlayerState, PlayerState->CheckpointSound);
					PlayerState->CurrentCheckpointIndex = (PlayerState->CurrentCheckpointIndex + 1) % TotalNumberOfCheckpoints;
					PlayerState->NextCheckpointIndex = (PlayerState->CurrentCheckpointIndex + 1) % TotalNumberOfCheckpoints;
					if (auto HUD2 = Cast<APlayerController>(OtherActor->GetInstigatorController())->GetHUD<AHUD2>()) {
						HUD2->CurrentCheckpoint = FWaypoint(*Checkpoints[PlayerState->CurrentCheckpointIndex]);
						HUD2->NextCheckpoint = FWaypoint(*Checkpoints[PlayerState->NextCheckpointIndex]);
					}
				}
			}
		}
	}
}
void AGameStateBase2::ResetTrack() {
	if (auto World = GetWorld()) {
		Checkpoints.Empty();
		// todo have the server be in charge of checkpoint indexes (validations, replication, etc)
		for (TActorIterator<ACheckpoint> i(World); i; ++i) {
			i->CheckpointTrigger->OnComponentBeginOverlap.AddDynamic(this, &AGameStateBase2::OnOverlap);
			Checkpoints.Add(*i);
		}
		TotalNumberOfCheckpoints = Checkpoints.Num();
		if (TotalNumberOfCheckpoints > 2) {
			Checkpoints.Sort();
			for (size_t i = 0; i < TotalNumberOfCheckpoints; i++) {
				Checkpoints[i]->CheckpointIndex = i;
			}
			for (TActorIterator<AHUD2> i(World); i; ++i) {
				i->CurrentCheckpoint = FWaypoint(*Checkpoints[0]);
				i->NextCheckpoint = FWaypoint(*Checkpoints[1]);
				if (i->GetOwningPawn()) {
					i->bShowCheckpointMarkers = true;
				}
			}
		}
	}
}
