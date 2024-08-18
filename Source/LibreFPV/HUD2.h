// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameStateBase2.h"
#include "HUD2.generated.h"

class SConstraintCanvas;
class SQuadcopterSettingsWidget;
class UInputComponent;

USTRUCT(BlueprintType)
struct FWaypoint {
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction;

	FWaypoint() {
		Location = FVector::ZeroVector;
		Direction = FVector::ZeroVector;
	}
	FWaypoint(const AActor& A) {
		Location = A.GetActorLocation();
		Direction = A.GetActorForwardVector();
	}
};

UCLASS()
class LIBREFPV_API AHUD2 : public AHUD
{
	GENERATED_BODY()
public:
	AHUD2();
	void Tick(float DeltaTime) override;
	void DrawHUD() override;

	void CreateHud();

	TSharedPtr<SConstraintCanvas> PlayerSlateHud;

	TSharedPtr<STextBlock> FpsDisplay;
	TSharedPtr<STextBlock> CheckpointSplit;

	float FpsBuffer;
	float FpsSamples;
	FNumberFormattingOptions NumberFormat;

	bool bShowCheckpointMarkers;
	FWaypoint CurrentCheckpoint;
	FWaypoint NextCheckpoint;
	FVector2D RectagleSize;
	void DrawCheckpointMarker(FWaypoint& Checkpoint, bool bIsGrey = false);

	void ToggleEscapeMenu();
	bool bEscapeMenuIsOpen;
	TSharedPtr<SQuadcopterSettingsWidget> EscapeMenu;
};
