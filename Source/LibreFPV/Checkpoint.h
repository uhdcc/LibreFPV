// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class LIBREFPV_API ACheckpoint : public AActor {
	GENERATED_BODY()	
public:	
	ACheckpoint();
	bool operator<(const ACheckpoint& Other) const {
		return GetFName().LexicalLess(Other.GetFName());
	}
	virtual void OnConstruction(const FTransform& Transform) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* CheckpointTrigger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CheckpointModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* CheckpointArrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CheckpointIndex;

};
