// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD2.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Engine/Canvas.h"
#include "SQuadcopterSettingsWidget.h"
#include "Quadcopter.h"


AHUD2::AHUD2() {
	SetActorTickEnabled(false);
	FpsBuffer = 0.f;
	FpsSamples = 0.f;
	NumberFormat.MinimumFractionalDigits = 4;
	NumberFormat.UseGrouping = false;
	bShowCheckpointMarkers = false;
	RectagleSize = FVector2D(20.f);
	bEscapeMenuIsOpen = false;
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
void AHUD2::CreateHud() {
	InputComponent->BindAction("EscapeMenu", IE_Pressed, this, &AHUD2::ToggleEscapeMenu);
	// canvas for all hud widgets
	GEngine->GameViewport->AddViewportWidgetForPlayer(
		GetOwningPlayerController()->GetLocalPlayer(),
		SAssignNew(PlayerSlateHud, SConstraintCanvas),
		0
	);
	// fps display
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
	// checkpoint split
	PlayerSlateHud->AddSlot()
		.Anchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f))
		.Alignment(FVector2D(0.5f, -2.f))
		.AutoSize(true)
		[
			SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.02f, 1.f))
				[
					SAssignNew(CheckpointSplit, STextBlock)
						.Font(FCoreStyle::GetDefaultFontStyle("Light", 20))
						.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f, 1.f))
				]
		];



	if (GetOwningPawn()) {
		SetActorTickEnabled(true);
	}
}
void AHUD2::DrawCheckpointMarker(FWaypoint& Checkpoint, bool bIsGrey) {
	// project checkpoint location to screen
	auto ScreenSpaceVector = Project(Checkpoint.Location);
	// if checkpoint is in front of the player...
	if (ScreenSpaceVector.Z != 0.f) {
		// determine marker's color
		bool bIsGreen = false;
		if (!bIsGrey) {
			auto DotProduct = FVector::DotProduct(Checkpoint.Location - GetOwningPawn()->GetActorLocation(), Checkpoint.Direction);
			bIsGreen = (DotProduct > 0.f);
		}
		// draw checkpoint marker
		DrawRect(
			bIsGrey ? FLinearColor::Gray : bIsGreen ? FLinearColor::Green : FLinearColor::Red,
			ScreenSpaceVector.X - RectagleSize.X, ScreenSpaceVector.Y - RectagleSize.Y,
			RectagleSize.X,
			RectagleSize.Y
		);
	}
}
void AHUD2::ToggleEscapeMenu() {
	if (bEscapeMenuIsOpen) {
		// hide menu and enable input, hide mouse cursor
		EscapeMenu->SetVisibility(EVisibility::Hidden);
		GetOwningPlayerController()->bShowMouseCursor = false;
		GetOwningPawn()->EnableInput(GetOwningPlayerController());
		GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
	}
	else {
		if (!EscapeMenu) {
			if (GetOwningPawn()) {
				if (auto Quadcopter = Cast<AQuadcopter>(GetOwningPawn())) {
					// create escape menu
					PlayerSlateHud->AddSlot()
						.Anchors(FAnchors(1.f, 0.5f, 1.f, 0.5f))
						.Alignment(FVector2D(1.f, 0.5f))
						.AutoSize(true)
						[
							SAssignNew(EscapeMenu, SQuadcopterSettingsWidget)
								.Quadcopter(Quadcopter)
						];
				}
			}
		}
		else {
			// unhide escape menu
			EscapeMenu->SetVisibility(EVisibility::Visible);
		}
		// disable input, show mouse cursor
		GetOwningPlayerController()->bShowMouseCursor = true;
		GetOwningPawn()->DisableInput(GetOwningPlayerController());
		GetOwningPlayerController()->SetInputMode(FInputModeGameAndUI());
	}
	bEscapeMenuIsOpen = !bEscapeMenuIsOpen;
}
