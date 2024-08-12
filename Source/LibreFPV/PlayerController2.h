// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController2.generated.h"

UCLASS()
class LIBREFPV_API APlayerController2 : public APlayerController
{
	GENERATED_BODY()
public:
	APlayerController2();
	virtual void SetPawn(APawn* InPawn) override;
};
