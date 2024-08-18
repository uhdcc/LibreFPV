// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Quadcopter.generated.h"

class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWantsRestartRun, AQuadcopter*, Quadcopter);


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
		Speed = 475.f;
		Precision = 0.6f;
		Transition = 0.3f;
		Deadzone = 0.05f;	}
};

UCLASS()
class LIBREFPV_API AQuadcopter : public APawn{
	GENERATED_BODY()
public:
	AQuadcopter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Throttle(float Input);
	void Pitch(float Input);
	void Yaw(float Input);
	void Roll(float Input);
	void KeyboardYaw(float Input);
	void MousePitch(float Input);
	void MouseRoll(float Input);

	bool GamepadDeadzone(float& AxisInput);
	void GamepadCurve(float& AxisInput);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* QuadcopterCamera;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RelativeInput;
	bool bHasRelativeRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PropellerDistance;

	double MouseSensitivity;
	void SetMouseSensitivity(double MouseDpi, double CentimetersPer360);

	void RestartRun();
	FVector InitialLocation;
	FRotator InitialRotation;

	UPROPERTY(BlueprintAssignable)
	FWantsRestartRun	WantsRestartRun;
};
