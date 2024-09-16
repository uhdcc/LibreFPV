// Fill out your copyright notice in the Description page of Project Settings.


#include "Quadcopter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameStateBase2.h"
#include "PlayerConfig.h"

AQuadcopter::AQuadcopter() {
	PrimaryActorTick.bCanEverTick = true;

	QuadcopterCollision = CreateDefaultSubobject<UBoxComponent>("QuadcopterCollision");
	QuadcopterCollision->SetBoxExtent(FVector(38.f, 38.f, 3.5f));
	QuadcopterCollision->SetCollisionProfileName("BlockAllDynamic");
	QuadcopterCollision->SetSimulatePhysics(true);
	QuadcopterCollision->SetLinearDamping(1.5f);
	QuadcopterCollision->SetMassOverrideInKg(NAME_None, 1.f, true);
	//QuadcopterCollision->SetCenterOfMass(FVector(0.f, 0.f, 3.f));
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
	LegacyRates = nullptr;
}
void AQuadcopter::PostInitializeComponents() {
	Super::PostInitializeComponents();
	LegacyRates = NewObject<ULegacyRates>();
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
	PlayerInputComponent->BindAction("RunDebugFunction", IE_Pressed, this, &AQuadcopter::RunDebugFunction);

}
void AQuadcopter::Throttle(float Input) {
		for (int i = 0; i < THROTTLE_LOOKUP_LENGTH; i++) {
		const int16_t tmp = 10 * i - LegacyRates->currentControlRateProfile.thrMid8;
		uint8_t y = 1;
		if (tmp > 0)
			y = 100 - LegacyRates->currentControlRateProfile.thrMid8;
		if (tmp < 0)
			y = LegacyRates->currentControlRateProfile.thrMid8;
		LegacyRates->lookupThrottleRC[i] = 10 * LegacyRates->currentControlRateProfile.thrMid8 + tmp * (100 - LegacyRates->currentControlRateProfile.thrExpo8 + (int32_t)LegacyRates->currentControlRateProfile.thrExpo8 * (tmp * tmp) / (y * y)) / 10;
		LegacyRates->lookupThrottleRC[i] = PWM_RANGE_MIN + PWM_RANGE * LegacyRates->lookupThrottleRC[i] / 1000; // [MINTHROTTLE;MAXTHROTTLE]
	}

	//QuadcopterCollision->SetLinearDamping(FMath::Min(1.0f, Input + 0.8f));
	if (!IsMoveInputIgnored()) {
		GamepadDeadzone(Input);
		ThrottleInput = Input;
	}
	else {
		ThrottleInput = 0.f;
	}
}
void AQuadcopter::Pitch(float Input) {
	GamepadInput(Input, 0);
}
void AQuadcopter::Yaw(float Input) {
	GamepadInput(Input,1);
}
void AQuadcopter::Roll(float Input) {
	GamepadInput(Input, 2);
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
void AQuadcopter::GamepadInput(float& Input, int AxisIndex) {
	if (Input == 0.f || IsMoveInputIgnored()) return;
	GamepadDeadzone(Input);

	//for (int i = 0; i < THROTTLE_LOOKUP_LENGTH; i++) {
	//	const int16_t tmp = 10 * i - LegacyRates->currentControlRateProfile.thrMid8;
	//	uint8_t y = 1;
	//	if (tmp > 0)
	//		y = 100 - LegacyRates->currentControlRateProfile.thrMid8;
	//	if (tmp < 0)
	//		y = LegacyRates->currentControlRateProfile.thrMid8;
	//	LegacyRates->lookupThrottleRC[i] = 10 * LegacyRates->currentControlRateProfile.thrMid8 + tmp * (100 - LegacyRates->currentControlRateProfile.thrExpo8 + (int32_t)LegacyRates->currentControlRateProfile.thrExpo8 * (tmp * tmp) / (y * y)) / 10;
	//	LegacyRates->lookupThrottleRC[i] = PWM_RANGE_MIN + PWM_RANGE * LegacyRates->lookupThrottleRC[i] / 1000; // [MINTHROTTLE;MAXTHROTTLE]
	//}

	float AbsoluteInput = abs(Input);
	switch (LegacyRates->currentControlRateProfile.rates_type) {
		case RATES_TYPE_BETAFLIGHT:
			Input = LegacyRates->applyBetaflightRates(AxisIndex, Input, AbsoluteInput);
			break;
		case RATES_TYPE_RACEFLIGHT:
			Input = LegacyRates->applyRaceFlightRates(AxisIndex, Input, AbsoluteInput);
			break;
		case RATES_TYPE_KISS:
			Input = LegacyRates->applyKissRates(AxisIndex, Input, AbsoluteInput);
			break;
		case RATES_TYPE_ACTUAL:
			Input = LegacyRates->applyActualRates(AxisIndex, Input, AbsoluteInput);
			break;
		case RATES_TYPE_QUICK:
			Input = LegacyRates->applyQuickRates(AxisIndex, Input, AbsoluteInput);
			break;
	}
	switch(AxisIndex){
		case 0:
			RelativeInput.Pitch += Input;
			break;
		case 1:
			RelativeInput.Yaw += Input;
			break;
		case 2:
			RelativeInput.Roll += Input;
			break;
	}
	bHasRelativeRotation = true;
}

void AQuadcopter::RunDebugFunction() {
	UPlayerConfig::LoadPlayerConfig(*this, *GetController<APlayerController>());
}
