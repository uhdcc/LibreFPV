// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LibreFPVGameModeBase.generated.h"

class AGameStateBase2;

UCLASS()
class LIBREFPV_API ALibreFPVGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ALibreFPVGameModeBase();
	virtual void BeginPlay() override;
	void ResetTrack();
	AGameStateBase2* GameState2;
};
