// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugInputActor.h"

ADebugInputActor::ADebugInputActor() {
	AxisValues.Init(0.f, 8);
}

void ADebugInputActor::EnableInput(APlayerController* PlayerController) {
	Super::EnableInput(PlayerController);
	if(!InputComponent) return;
	InputComponent->BindAxis("DebugAxis1", this, &ADebugInputActor::DebugAxis1);
	InputComponent->BindAxis("DebugAxis2", this, &ADebugInputActor::DebugAxis2);
	InputComponent->BindAxis("DebugAxis3", this, &ADebugInputActor::DebugAxis3);
	InputComponent->BindAxis("DebugAxis4", this, &ADebugInputActor::DebugAxis4);
	InputComponent->BindAxis("DebugAxis5", this, &ADebugInputActor::DebugAxis5);
	InputComponent->BindAxis("DebugAxis6", this, &ADebugInputActor::DebugAxis6);
	InputComponent->BindAxis("DebugAxis7", this, &ADebugInputActor::DebugAxis7);
	InputComponent->BindAxis("DebugAxis8", this, &ADebugInputActor::DebugAxis8);
}

void ADebugInputActor::DebugAxis1(float Input) {
	AxisValues[0] = (Input + 1.f) * 0.5f;
}

void ADebugInputActor::DebugAxis2(float Input) {
	AxisValues[1] = (Input + 1.f) * 0.5f;

}

void ADebugInputActor::DebugAxis3(float Input) {
	AxisValues[2] = (Input + 1.f) * 0.5f;

}

void ADebugInputActor::DebugAxis4(float Input) {
	AxisValues[3] = (Input + 1.f) * 0.5f;

}

void ADebugInputActor::DebugAxis5(float Input) {
	AxisValues[4] = (Input + 1.f) * 0.5f;

}

void ADebugInputActor::DebugAxis6(float Input) {
	AxisValues[5] = (Input + 1.f) * 0.5f;

}

void ADebugInputActor::DebugAxis7(float Input) {
	AxisValues[6] = (Input + 1.f) * 0.5f;

}

void ADebugInputActor::DebugAxis8(float Input) {
	AxisValues[7] = (Input + 1.f) * 0.5f;

}

