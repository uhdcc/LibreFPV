// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Windows/WindowsApplication.h"
#include "PlayerController2.generated.h"

/**
 * 
 */
UCLASS()
class LIBREFPV_API APlayerController2 : public APlayerController/*, public IWindowsMessageHandler*/
{
	GENERATED_BODY()
public:
	void BeginPlay() override;
	void SetPawn(APawn* InPawn) override;
};
