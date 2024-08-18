// Fill out your copyright notice in the Description page of Project Settings.


#include "Quadcopter.h"
#include "Camera/CameraComponent.h"

AQuadcopter::AQuadcopter() {
	PrimaryActorTick.bCanEverTick = true;

	QuadcopterCollision = CreateDefaultSubobject<UStaticMeshComponent>("QuadcopterCollision");
	QuadcopterCollision->SetSimulatePhysics(true);
	RootComponent = QuadcopterCollision;

	QuadcopterCamera = CreateDefaultSubobject<UCameraComponent>("QuadcopterCamera");
	QuadcopterCamera->bConstrainAspectRatio = true;
	QuadcopterCamera->FieldOfView = 110.f;
	QuadcopterCamera->SetRelativeRotation(FRotator(45.f, 0.f, 0.f));
	QuadcopterCamera->SetupAttachment(RootComponent);

	ThrottleInput = 0.f;
	ThrottleForce = 1200.f;
	RotationInput = FRotator::ZeroRotator;
	bHasRelativeRotation = false;
	MaxSpeed = 2000.f;
	PropellerDistance = 20.f;
	SetMouseSensitivity(800.0, 8.0);
	InitialLocation = FVector::ZeroVector;
	InitialRotation = FRotator::ZeroRotator;

}
void AQuadcopter::BeginPlay() {
	Super::BeginPlay();	
}
void AQuadcopter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (ThrottleInput > 0.f) {
		auto InstantaneousForce = FVector(0.f, 0.f, ThrottleInput * ThrottleForce);
		if (PropellerDistance == 0.f) {
			QuadcopterCollision->AddForce(InstantaneousForce * 4.f);
		}
		else {
			QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(PropellerDistance, PropellerDistance, 0.f));
			QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(-PropellerDistance, PropellerDistance, 0.f));
			QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(PropellerDistance, -PropellerDistance, 0.f));
			QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(-PropellerDistance, -PropellerDistance, 0.f));
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
