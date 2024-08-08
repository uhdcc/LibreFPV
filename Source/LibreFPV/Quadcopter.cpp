// Fill out your copyright notice in the Description page of Project Settings.


#include "Quadcopter.h"

// Sets default values
AQuadcopter::AQuadcopter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	QuadcopterCollision = CreateDefaultSubobject<UStaticMeshComponent>("QuadcopterCollision");
	QuadcopterCollision->SetSimulatePhysics(true);
	ThrottleInput = 0.f;
	ThrottleForce = 1200.f;
	RotationInput = FRotator::ZeroRotator;
	bHasRotationInput = false;
	ThrottleCurve = nullptr;
	MaxSpeed = 2000.f;
	PropellerDistance = 5.f;
}
void AQuadcopter::BeginPlay() {
	Super::BeginPlay();	
}
void AQuadcopter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, LexToString(QuadcopterCollision->GetPhysicsLinearVelocity().Size()));

	if (ThrottleInput > 0.f) {
		auto InstantaneousForce = FVector(0.f, 0.f, ThrottleInput * ThrottleForce);
		//auto InstantaneousForce = FVector(
		//	0.f,
		//	0.f,
		//	ThrottleInput * ThrottleForce * ThrottleCurve->GetFloatValue(QuadcopterCollision->GetPhysicsLinearVelocity().Size())
		//);
		QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(PropellerDistance, PropellerDistance, 0.f));
		QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(-PropellerDistance, PropellerDistance, 0.f));
		QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(PropellerDistance, -PropellerDistance, 0.f));
		QuadcopterCollision->AddForceAtLocationLocal(InstantaneousForce, FVector(-PropellerDistance, -PropellerDistance, 0.f));

		if (QuadcopterCollision->GetPhysicsLinearVelocity().Size() > MaxSpeed) {
			QuadcopterCollision->SetPhysicsLinearVelocity(QuadcopterCollision->GetPhysicsLinearVelocity().GetSafeNormal() * MaxSpeed);
		}
	}
	if (bHasRotationInput) {
		AddActorLocalRotation(RotationInput * DeltaTime);
		bHasRotationInput = false;
	}
}
void AQuadcopter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Throttle", this, &AQuadcopter::Throttle);
	PlayerInputComponent->BindAxis("Pitch", this, &AQuadcopter::Pitch);
	PlayerInputComponent->BindAxis("Yaw", this, &AQuadcopter::Yaw);
	PlayerInputComponent->BindAxis("Roll", this, &AQuadcopter::Roll);


}
void AQuadcopter::Throttle(float Input) {
	GamepadDeadzone(Input);
	ThrottleInput = Input;
}

void AQuadcopter::Pitch(float Input) {
	if (Input != 0.f) {
		if (GamepadDeadzone(Input)) {
			GamepadCurve(Input);
			RotationInput.Pitch = Input;
			bHasRotationInput = true;
		}
	}
}

void AQuadcopter::Yaw(float Input) {
	if (Input != 0.f) {
		if (GamepadDeadzone(Input)) {
			GamepadCurve(Input);
			RotationInput.Yaw = Input;
			bHasRotationInput = true;
		}
	}
}

void AQuadcopter::Roll(float Input) {
	if (Input != 0.f) {
		if (GamepadDeadzone(Input)) {
			GamepadCurve(Input);
			RotationInput.Roll = Input;
			bHasRotationInput = true;
		}
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

