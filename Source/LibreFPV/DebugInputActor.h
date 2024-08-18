// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DebugInputActor.generated.h"

UCLASS()
class LIBREFPV_API ADebugInputActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADebugInputActor();
	virtual void EnableInput(class APlayerController* PlayerController);

	void DebugAxis1(float Input);
	void DebugAxis2(float Input);
	void DebugAxis3(float Input);
	void DebugAxis4(float Input);
	void DebugAxis5(float Input);
	void DebugAxis6(float Input);
	void DebugAxis7(float Input);
	void DebugAxis8(float Input);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> AxisValues;
};
