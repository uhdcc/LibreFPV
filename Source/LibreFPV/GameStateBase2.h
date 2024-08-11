// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameStateBase2.generated.h"

class ACheckpoint;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACheckpoint*> Checkpoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalNumberOfCheckpoints;
	UFUNCTION(BlueprintCallable)
	void ResetTrack();
};
