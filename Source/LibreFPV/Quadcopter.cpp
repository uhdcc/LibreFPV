// Fill out your copyright notice in the Description page of Project Settings.


#include "Quadcopter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"

AQuadcopter::AQuadcopter() {
	PrimaryActorTick.bCanEverTick = true;

	QuadcopterCollision = CreateDefaultSubobject<UBoxComponent>("QuadcopterCollision");
	QuadcopterCollision->SetBoxExtent(FVector(38.f, 38.f, 3.5f));
	QuadcopterCollision->SetCollisionProfileName("BlockAllDynamic");
	QuadcopterCollision->SetSimulatePhysics(true);
	QuadcopterCollision->SetLinearDamping(1.f);
	QuadcopterCollision->SetMassOverrideInKg(NAME_None, 1.f, true);
	QuadcopterCollision->SetCenterOfMass(FVector(0.f, 0.f, 3.f));
	QuadcopterCollision->GetBodyInstance()->PositionSolverIterationCount = 16;
	QuadcopterCollision->GetBodyInstance()->VelocitySolverIterationCount = 8;
	QuadcopterCollision->GetBodyInstance()->InertiaTensorScale = FVector::ZeroVector;
	QuadcopterCollision->GetBodyInstance()->StabilizationThresholdMultiplier = 0.f;
	RootComponent = QuadcopterCollision;

	QuadcopterModel = CreateDefaultSubobject<UStaticMeshComponent>("QuadcopterModel");
	QuadcopterModel->SetCollisionProfileName("NoCollision");
	QuadcopterModel->SetOwnerNoSee(true);
	QuadcopterModel->SetRelativeLocation(FVector(0.f, 0.f, -3.1f));
	QuadcopterModel->SetupAttachment(RootComponent);

	QuadcopterCamera = CreateDefaultSubobject<UCameraComponent>("QuadcopterCamera");
	QuadcopterCamera->bConstrainAspectRatio = true;
	QuadcopterCamera->FieldOfView = 130.f;
	QuadcopterCamera->SetRelativeRotation(FRotator(53.f, 0.f, 0.f));
	QuadcopterCamera->SetRelativeLocation(FVector(8.f, 0.f, 4.f));
	QuadcopterCamera->SetRelativeScale3D(FVector(0.3f));
	QuadcopterCamera->SetupAttachment(RootComponent);

	ThrottleInput = 0.f;
	ThrusterForce = 1200.f;
	RotationInput = FRotator::ZeroRotator;
	bHasRelativeRotation = false;
	MaxSpeed = 2000.f;
	ThrusterDistance = 7.f;
	SetMouseSensitivity(800.0, 8.0);
	InitialLocation = FVector::ZeroVector;
	InitialRotation = FRotator::ZeroRotator;
	ThrusterOffset = -2.f;
}
void AQuadcopter::BeginPlay() {
	Super::BeginPlay();	
}
void AQuadcopter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (ThrottleInput > 0.f) {
		auto InstantaneousForce = FVector(0.f, 0.f, ThrottleInput * ThrusterForce);
		if (ThrusterDistance == 0.f) {
			QuadcopterCollision->AddForce(InstantaneousForce * 4.f);
		}
		else {
			QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(ThrusterDistance, ThrusterDistance, ThrusterOffset));
			QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(-ThrusterDistance, ThrusterDistance, ThrusterOffset));
			QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(ThrusterDistance, -ThrusterDistance, ThrusterOffset));
			QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(-ThrusterDistance, -ThrusterDistance, ThrusterOffset));
		}
		if (QuadcopterCollision->GetPhysicsLinearVelocity().Size() > MaxSpeed) {
			QuadcopterCollision->SetPhysicsLinearVelocity(QuadcopterCollision->GetPhysicsLinearVelocity().GetSafeNormal() * MaxSpeed);
		}
	}
	if (bHasRelativeRotation) {
		RelativeInput *= DeltaTime;
		RotationInput += RelativeInput;
		AddActorLocalRotation(RotationInput);
		RotationInput = FRotator::ZeroRotator;
		bHasRelativeRotation = false;
	}
}
void AQuadcopter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Throttle", this, &AQuadcopter::Throttle);
	PlayerInputComponent->BindAxis("Pitch", this, &AQuadcopter::Pitch);
	PlayerInputComponent->BindAxis("Yaw", this, &AQuadcopter::Yaw);
	PlayerInputComponent->BindAxis("Roll", this, &AQuadcopter::Roll);
	PlayerInputComponent->BindAxis("KeyboardYaw", this, &AQuadcopter::KeyboardYaw);
	PlayerInputComponent->BindAxis("MousePitch", this, &AQuadcopter::MousePitch);
	PlayerInputComponent->BindAxis("MouseRoll", this, &AQuadcopter::MouseRoll);
	PlayerInputComponent->BindAction("RestartRun", IE_Pressed, this, &AQuadcopter::RestartRun);
}
void AQuadcopter::Throttle(float Input) {
	if (!IsMoveInputIgnored()) {
		GamepadDeadzone(Input);
		ThrottleInput = Input;
	}
	else {
		ThrottleInput = 0.f;
	}
}
void AQuadcopter::Pitch(float Input) {
	if (Input != 0.f && !IsMoveInputIgnored()) {
		if (GamepadDeadzone(Input)) {
			GamepadCurve(Input);
			RelativeInput.Pitch += Input;
			bHasRelativeRotation = true;
		}
	}
}
void AQuadcopter::Yaw(float Input) {
	if (Input != 0.f && !IsMoveInputIgnored()) {
		if (GamepadDeadzone(Input)) {
			GamepadCurve(Input);
			RelativeInput.Yaw += Input;
			bHasRelativeRotation = true;
		}
	}
}
void AQuadcopter::Roll(float Input) {
	if (Input != 0.f && !IsMoveInputIgnored()) {
		if (GamepadDeadzone(Input)) {
			GamepadCurve(Input);
			RelativeInput.Roll += Input;
			bHasRelativeRotation = true;
		}
	}
}
void AQuadcopter::KeyboardYaw(float Input) {
	if (Input != 0.f && !IsMoveInputIgnored()) {
		RelativeInput.Yaw += Input * 200.f;
		bHasRelativeRotation = true;
	}
}
void AQuadcopter::MousePitch(float Input) {
	if (Input != 0.f && !IsMoveInputIgnored()) {
		RotationInput.Pitch += Input * MouseSensitivity;
		bHasRelativeRotation = true;
	}
}
void AQuadcopter::MouseRoll(float Input) {
	if (Input != 0.f && !IsMoveInputIgnored()) {
		RotationInput.Roll += Input * MouseSensitivity;
		bHasRelativeRotation = true;
	}
}
bool AQuadcopter::GamepadDeadzone(float& AxisInput) {
	float AbsInput = FMath::Abs(AxisInput);
	if (AbsInput > GamepadProperties.Deadzone) {
		AxisInput = FMath::Sign(AxisInput) * ((AbsInput - GamepadProperties.Deadzone) / (1.f - GamepadProperties.Deadzone));
		return true;
	}
	return false;
}
void AQuadcopter::GamepadCurve(float& AxisInput) {
	AxisInput = GamepadProperties.Speed * AxisInput * (GamepadProperties.Precision * FMath::Pow(FMath::Abs(AxisInput), 20.f * GamepadProperties.Transition) + (1.f - GamepadProperties.Precision));
}
void AQuadcopter::SetMouseSensitivity(double MouseDpi, double CentimetersPer360) {
	MouseSensitivity = 360.0 / MouseDpi / CentimetersPer360 * 2.54;
}

void AQuadcopter::RestartRun() {
	if (WantsRestartRun.IsBound()) WantsRestartRun.Broadcast(this);
}




void AQuadcopter::GamepadInput(float Input, int AxisIndex) {
	if (Input = 0.f || IsMoveInputIgnored()) return;
	if (bUsesLegacyRates) {
		switch (LegacyRatesType) {
			float AbsoluteInput = abs(Input);
			case RATES_TYPE_BETAFLIGHT:
				Input = applyBetaflightRates(AxisIndex, Input, AbsoluteInput);
				break;
			case RATES_TYPE_RACEFLIGHT:
				Input = applyRaceFlightRates(AxisIndex, Input, AbsoluteInput);
				break;
			case RATES_TYPE_KISS:
				Input = applyKissRates(AxisIndex, Input, AbsoluteInput);
				break;
			case RATES_TYPE_ACTUAL:
				Input = applyActualRates(AxisIndex, Input, AbsoluteInput);
				break;
			case RATES_TYPE_QUICK:
				Input = applyQuickRates(AxisIndex, Input, AbsoluteInput);
				break;
		}
	}
	else {
		if (GamepadDeadzone(Input)) {
			GamepadCurve(Input);
		}
	}

	RelativeInput.Roll += Input;
	bHasRelativeRotation = true;
}


