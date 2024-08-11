// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD2.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Engine/Canvas.h"

AHUD2::AHUD2() {
	SetActorTickEnabled(false);
	FpsBuffer = 0.f;
	FpsSamples = 0.f;
	NumberFormat.MinimumFractionalDigits = 4;
	NumberFormat.UseGrouping = false;
	bShowCheckpointMarkers = false;
	RectagleSize = FVector2D(20.f);
}
void AHUD2::BeginPlay() {
	Super::BeginPlay();
	GEngine->GameViewport->AddViewportWidgetForPlayer(
		GetOwningPlayerController()->GetLocalPlayer(),
		SAssignNew(PlayerSlateHud, SConstraintCanvas),
		0
	);
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(0.f, 0.f, 0.f, 0.f))
		.Alignment(FVector2D(0.f, 0.f))
		.AutoSize(true)
		[
			SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.02f, 1.f))
				[
				SAssignNew(FpsDisplay, STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Light", 20))
					.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.f))
				]
		];
	if (GetOwningPawn() && GetWorld()) {
		SetActorTickEnabled(true);
	}
}
void AHUD2::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);	
	FpsBuffer += DeltaTime;
	FpsSamples++;
	if (FpsBuffer > 0.03f) {
		FpsDisplay->SetText(FText::FromString(FText::AsNumber(1.f / (FpsBuffer / FpsSamples), &NumberFormat).ToString() + " FPS"));
		FpsBuffer = 0.f;
		FpsSamples = 0;
	}
}
void AHUD2::DrawHUD() {
	Super::DrawHUD();
	if (bShowCheckpointMarkers) {
		DrawCheckpointMarker(CurrentCheckpoint);
		DrawCheckpointMarker(NextCheckpoint, true);
	}
}
void AHUD2::DrawCheckpointMarker(FWaypoint& Checkpoint, bool bIsGrey) {
	auto ScreenSpaceVector = Project(Checkpoint.Location);
	if (ScreenSpaceVector.Z != 0.f) {
		bool bIsGreen = false;
		if (!bIsGrey) {
			auto DotProduct = FVector::DotProduct(Checkpoint.Location - GetOwningPawn()->GetActorLocation(), Checkpoint.Direction);
			bIsGreen = (DotProduct > 0.f);
		}
		DrawRect(
			bIsGrey ? FLinearColor::Gray : bIsGreen ? FLinearColor::Green : FLinearColor::Red,
			ScreenSpaceVector.X - RectagleSize.X, ScreenSpaceVector.Y - RectagleSize.Y,
			RectagleSize.X,
			RectagleSize.Y
		);
	}
}
