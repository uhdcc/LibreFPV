// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Quadcopter.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct FGamepadProperties {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Precision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Transition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Deadzone;

	FGamepadProperties() {
		Speed = 320.f;
		Precision = 0.8f;
		Transition = 0.25f;
		Deadzone = 0.1f;	}
};

UCLASS()
class LIBREFPV_API AQuadcopter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AQuadcopter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Throttle(float Input);
	void Pitch(float Input);
	void Yaw(float Input);
	void Roll(float Input);

	bool GamepadDeadzone(float& AxisInput);

	void GamepadCurve(float& AxisInput);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* QuadcopterCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrottleInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThrottleForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGamepadProperties GamepadProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationInput;
	bool bHasRotationInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* ThrottleCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PropellerDistance;
};
