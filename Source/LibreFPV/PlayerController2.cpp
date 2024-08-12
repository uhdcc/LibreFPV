// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController2.h"
#include "HUD2.h"

APlayerController2::APlayerController2() {
}
void APlayerController2::SetPawn(APawn* InPawn) {
	Super::SetPawn(InPawn);
	if (IsLocalPlayerController() && GetHUD()) {
		if (auto HUD2 = Cast<AHUD2>(GetHUD())) {
			HUD2->EnableInput(this);
			HUD2->CreateHud();
		}
	}
}
