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

#include "PlayerConfig.h"

#include "D:\Apps\Epic Games\UE_4.27\Engine\Plugins\AI\UE4ML\Source\UE4ML\Public\4MLJson.h"

#include "Camera/CameraComponent.h"


AGameStateBase2::AGameStateBase2() {
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	TotalNumberOfCheckpoints = 0;
	bResetTrack = false;
	bReceivedCheckpoints = false;
	bReceivedStartingBlocks = false;
	bPreviousCheckpointColor = false;
	LocalQuadcopter = nullptr;
	LocalPlayerState = nullptr;

	LocalPlayerConfig = NewObject<UPlayerConfig>(this, "LocalPlayerConfig");
}
void AGameStateBase2::BeginPlay() {
	Super::BeginPlay();
}
void AGameStateBase2::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
		auto DotProduct = FVector::DotProduct(
			CurrentCheckpointRay.Origin - LocalQuadcopter->GetActorLocation(),
			CurrentCheckpointRay.Direction
		);
		if ((DotProduct > 0.f) != bPreviousCheckpointColor) {
			Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->SetTriggerColor(
				bPreviousCheckpointColor ? FLinearColor::Red : FLinearColor::Green
			);
			bPreviousCheckpointColor = !bPreviousCheckpointColor;
		}
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
						Checkpoint->CheckpointTrigger->SetHiddenInGame(true);
						Checkpoint->SetTriggerColor(FLinearColor::Gray);
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
						Checkpoints[PlayerState->NextCheckpointIndex]->CheckpointTrigger->SetHiddenInGame(false);
						CurrentCheckpointRay.Origin = Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->GetActorLocation();
						CurrentCheckpointRay.Direction = Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->GetActorForwardVector();
						auto DotProduct = FVector::DotProduct(
							CurrentCheckpointRay.Origin - LocalQuadcopter->GetActorLocation(),
							CurrentCheckpointRay.Direction
						);
						bPreviousCheckpointColor = (DotProduct > 0.f);
						Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->SetTriggerColor(
							bPreviousCheckpointColor ? FLinearColor::Green : FLinearColor::Red
						);
						// update player hud
						if (auto HUD2 = Cast<APlayerController>(OtherActor->GetInstigatorController())->GetHUD<AHUD2>()) {
							HUD2->CurrentCheckpoint = Checkpoints[PlayerState->CurrentCheckpointIndex];
							HUD2->NextCheckpoint = Checkpoints[PlayerState->NextCheckpointIndex];
							if (CheckpointIndex > 0) HUD2->CheckpointSplit->UpdateCheckpointSplit(SplitDifference);
						}
					}
				}
			}
		}
	}
}

void AGameStateBase2::RestartRun(AQuadcopter* Quadcopter) {
		Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->CheckpointTrigger->SetHiddenInGame(true);
		Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->SetTriggerColor(FLinearColor::Gray);
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
		LocalPlayerState->CurrentCheckpointIndex = 0;
		LocalPlayerState->NextCheckpointIndex = 1;
		// todo: dont use FPlatformTime (this is real-world time). Use game time instead, so higher framerates dont trigger checkpoints sooner than low framerates
		LocalPlayerState->RunStartTime = FPlatformTime::Seconds();
		Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->CheckpointTrigger->SetHiddenInGame(false);
		Checkpoints[LocalPlayerState->NextCheckpointIndex]->CheckpointTrigger->SetHiddenInGame(false);
		CurrentCheckpointRay.Origin = Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->GetActorLocation();
		CurrentCheckpointRay.Direction = Checkpoints[LocalPlayerState->CurrentCheckpointIndex]->GetActorForwardVector();
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
				UMaterial* CheckpointGlowMaterial = nullptr;
				for (auto i : Checkpoints) {
					if (!i->CheckpointTriggerGlow) {
						if (!CheckpointGlowMaterial) {
							CheckpointGlowMaterial = LoadObject<UMaterial>(
								nullptr,
								TEXT("Material'/Game/Checkpoints/CheckpointAssets/CheckpointTriggerMaterial.CheckpointTriggerMaterial'")
							);
						}
						i->CheckpointTriggerGlow = i->CheckpointTrigger->CreateAndSetMaterialInstanceDynamicFromMaterial(0,CheckpointGlowMaterial);
						i->SetTriggerColor(FLinearColor::Gray);
					}
					i->CheckpointTrigger->OnComponentBeginOverlap.AddDynamic(this, &AGameStateBase2::OnOverlap);
					//i->SetHasObserver();
				}
				// initialize players
				for (TActorIterator<APlayerController> i(World); i; ++i) {
					if (i->IsLocalPlayerController()) {
						if (auto Quadcopter = Cast<AQuadcopter>(i->GetPawn())) {
							LocalPlayerState = Cast<APlayerState2>(i->PlayerState);
							LocalQuadcopter = Quadcopter;
							Quadcopter->WantsRestartRun.AddDynamic(this, &AGameStateBase2::RestartRun);
							LocalPlayerConfig->LoadPlayerConfig(*LocalQuadcopter, **i);
							RestartRun(Quadcopter);
							break;
						}
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
