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
void APlayerController2::SetPawn(APawn* InPawn) {
	Super::SetPawn(InPawn);
	//if (IsLocalPlayerController()) {
	//	if (GetPawn()) {
	//		if (auto Quadcopter = Cast<AQuadcopter>(GetPawn())) {
	//			InputComponent->BindAxis("Throttle", Quadcopter, &AQuadcopter::Throttle);
	//		}
	//	}
	//}
}