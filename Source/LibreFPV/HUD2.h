// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameStateBase2.h"
#include "HUD2.generated.h"

class SConstraintCanvas;

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
	virtual void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void DrawHUD() override;
	TSharedPtr<SConstraintCanvas> PlayerSlateHud;

	TSharedPtr<STextBlock> FpsDisplay;
	float FpsBuffer;
	float FpsSamples;
	FNumberFormattingOptions NumberFormat;

	bool bShowCheckpointMarkers;
	FWaypoint CurrentCheckpoint;
	FWaypoint NextCheckpoint;
	FVector2D RectagleSize;
	void DrawCheckpointMarker(FWaypoint& Checkpoint, bool bIsGrey = false);
};
