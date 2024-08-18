// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameStateBase2.generated.h"

class ACheckpoint;
class AQuadcopter;

UCLASS()
class LIBREFPV_API AGameStateBase2 : public AGameStateBase
{
	GENERATED_BODY()
public:
	AGameStateBase2();
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UPROPERTY(ReplicatedUsing = OnRep_Checkpoints, Replicated, EditAnywhere, BlueprintReadWrite)
	TArray<ACheckpoint*> Checkpoints;
	UPROPERTY(ReplicatedUsing = OnRep_StartingBlocks, Replicated, EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> StartingBlocks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalNumberOfCheckpoints;
	UFUNCTION()
	void RestartRun(AQuadcopter* Quadcopter);

	UPROPERTY(ReplicatedUsing = OnRep_bResetTrack, BlueprintReadOnly)
	bool bResetTrack;
	UFUNCTION()
	virtual void OnRep_bResetTrack();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	UFUNCTION()
	virtual void OnRep_Checkpoints();
	bool bReceivedCheckpoints;
	UFUNCTION()
	virtual void OnRep_StartingBlocks();
	bool bReceivedStartingBlocks;
};
