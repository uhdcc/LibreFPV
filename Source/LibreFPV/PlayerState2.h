// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerState2.generated.h"

UCLASS()
class LIBREFPV_API APlayerState2 : public APlayerState {
	GENERATED_BODY()
public:
	APlayerState2();
	int CurrentCheckpointIndex;
	int NextCheckpointIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* CheckpointSound;
};
