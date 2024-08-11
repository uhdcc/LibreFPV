// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController2.h"
#include "Quadcopter.h"

void APlayerController2::BeginPlay() {
	Super::BeginPlay();
	if (IsLocalPlayerController()) {
		PlayerCameraManager->DefaultFOV = 110;
		PlayerCameraManager->bDefaultConstrainAspectRatio = true;
		PlayerCameraManager->DefaultAspectRatio = 1.777777f;
	}
}