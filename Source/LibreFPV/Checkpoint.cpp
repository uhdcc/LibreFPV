// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

ACheckpoint::ACheckpoint() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	CheckpointArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("CheckpointArrow"));
	CheckpointArrow->SetupAttachment(RootComponent);
	CheckpointTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckpointTrigger"));
	CheckpointTrigger->SetupAttachment(RootComponent);
	CheckpointTrigger->SetCollisionProfileName("OverlapAllDynamic");
	CheckpointModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointModel"));
	CheckpointModel->SetupAttachment(RootComponent);
	CheckpointIndex = -1;
}
void ACheckpoint::OnConstruction(const FTransform& Transform) {

}
