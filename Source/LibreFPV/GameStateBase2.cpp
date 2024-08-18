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
			// if this is the player's next checkpoint to trigger...
			auto CheckpointIndex = Cast<ACheckpoint>(OverlappedComponent->GetOwner())->GetCheckpointIndex();
			if (CheckpointIndex == PlayerState->CurrentCheckpointIndex) {				
				if (FVector::DotProduct(OverlappedComponent->GetForwardVector(), OtherActor->GetVelocity().GetSafeNormal()) > 0.f) {
					// play sound
					UGameplayStatics::PlaySound2D(PlayerState, PlayerState->CheckpointSound);
					// update playerstate
					auto SplitTime = FPlatformTime::Seconds() - PlayerState->RunStartTime;
					if (PlayerState->CheckpointSplits.IsValidIndex(PlayerState->CurrentCheckpointIndex)) {
						PlayerState->CheckpointSplits[PlayerState->CurrentCheckpointIndex] = SplitTime;
					}
					else {
						PlayerState->CheckpointSplits.Add(SplitTime);
					}
					PlayerState->CurrentCheckpointIndex = (PlayerState->CurrentCheckpointIndex + 1) % TotalNumberOfCheckpoints;
					PlayerState->NextCheckpointIndex = (PlayerState->CurrentCheckpointIndex + 1) % TotalNumberOfCheckpoints;
					// update player hud
					if (auto HUD2 = Cast<APlayerController>(OtherActor->GetInstigatorController())->GetHUD<AHUD2>()) {
						HUD2->CurrentCheckpoint = FWaypoint(*Checkpoints[PlayerState->CurrentCheckpointIndex]);
						HUD2->NextCheckpoint = FWaypoint(*Checkpoints[PlayerState->NextCheckpointIndex]);


						FNumberFormattingOptions NumberFormattingOptions;
						NumberFormattingOptions.SetUseGrouping(false);
						NumberFormattingOptions.MinimumIntegralDigits = 2;
						NumberFormattingOptions.MaximumIntegralDigits = 2;

						FText TimespanFormatPattern;
						FFormatNamedArguments TimeArguments;
						const auto Minutes = FMath::TruncToInt(SplitTime / 60.0f);
							TimespanFormatPattern = NSLOCTEXT("Timespan", "Format_MinutesSeconds", "{Minutes}:{Seconds}.{Milliseconds}");
							TimeArguments.Add(TEXT("Minutes"), FMath::TruncToInt(SplitTime / 60.0f));
						//if (Minutes > 0) {
						//}
						//else {
						//	TimespanFormatPattern = NSLOCTEXT("Timespan", "Format_MinutesSeconds", "{Seconds}.{Milliseconds}");
						//}
						TimeArguments.Add(TEXT("Seconds"), FText::AsNumber(FMath::TruncToInt(SplitTime) % 60, &NumberFormattingOptions));
						NumberFormattingOptions.MinimumIntegralDigits = 4;
						NumberFormattingOptions.MaximumIntegralDigits = 4;
						TimeArguments.Add(TEXT("Milliseconds"), FText::AsNumber(FMath::TruncToInt(FMath::Fractional(SplitTime) * 100), &NumberFormattingOptions));
						HUD2->CheckpointSplit->SetText(FText::Format(TimespanFormatPattern, TimeArguments));

					}
				}
			}
		}
	}
}

void AGameStateBase2::RestartRun(AQuadcopter* Quadcopter) {
	if (auto PlayerState = Cast<APlayerState2>(Cast<APlayerController>(Quadcopter->GetInstigatorController())->PlayerState)) {
		auto InitialLocation = FVector::ZeroVector;
		auto InitialRotation = FRotator::ZeroRotator;
		if (StartingBlocks.IsValidIndex(0)) {
			InitialLocation = StartingBlocks[0]->GetActorLocation();
			InitialRotation = StartingBlocks[0]->GetActorRotation();
		}
		Quadcopter->SetActorLocationAndRotation(InitialLocation, InitialRotation);
		Quadcopter->QuadcopterCollision->SetPhysicsLinearVelocity(FVector::ZeroVector);
		Quadcopter->QuadcopterCollision->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		PlayerState->CurrentCheckpointIndex = 0;
		PlayerState->NextCheckpointIndex = 1;
		PlayerState->RunStartTime = FPlatformTime::Seconds();
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
				// initialize hud
				for (TActorIterator<AHUD2> i(World); i; ++i) {
					i->CurrentCheckpoint = FWaypoint(*Checkpoints[0]);
					i->NextCheckpoint = FWaypoint(*Checkpoints[1]);
					// initialize quadcopter
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
