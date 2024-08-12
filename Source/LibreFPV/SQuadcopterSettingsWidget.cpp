// Fill out your copyright notice in the Description page of Project Settings.


#include "SQuadcopterSettingsWidget.h"
#include "SlateOptMacros.h"
#include "Camera/CameraComponent.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SQuadcopterSettingsWidget::Construct(const FArguments& InArgs) {
	Quadcopter = InArgs._Quadcopter;
	if (!Quadcopter.IsValid()) return;

	BackgroundColor = MakeShareable(new FLinearColor(0.015f, 0.015f, 0.015f, 1.f));
	TextColor = MakeShareable(new FLinearColor(0.7f, 0.7f, 0.7f, 1.f));
	SettingsEntryFont = FSlateFontInfo(FCoreStyle::GetDefaultFontStyle("Regular", 24));
	SettingsEntryFont.OutlineSettings.OutlineSize = 1;
	SpinboxStyle = MakeShareable(new FSpinBoxStyle());
	SpinboxStyle->BackgroundBrush = *FCoreStyle::Get().GetBrush("GenericWhiteBox");
	SpinboxStyle->BackgroundBrush.TintColor = BackgroundColor.ToSharedRef();
	SpinboxStyle->ForegroundColor = TextColor.ToSharedRef();
	SpinboxStyle->HoveredBackgroundBrush = *FCoreStyle::Get().GetBrush("GenericWhiteBox");
	SpinboxStyle->HoveredBackgroundBrush.TintColor = BackgroundColor.ToSharedRef();
	SpinboxStyle->InactiveFillBrush = FSlateNoResource();
	SpinboxStyle->ActiveFillBrush = FSlateNoResource();
	SpinboxStyle->ArrowsImage = FSlateNoResource();

	TSharedPtr<SVerticalBox> SettingsBox;
	ChildSlot
	[
		SAssignNew(SettingsBox, SVerticalBox)
	];
	NewFloatEntry(
		SettingsBox.ToSharedRef(), 
		FText::FromString("Camera Angle"), 
		Quadcopter->QuadcopterCamera->GetRelativeRotation().Pitch,
		FOnFloatValueChanged::CreateSP(this, &SQuadcopterSettingsWidget::CameraAngleChanged)
	);
	NewFloatEntry(
		SettingsBox.ToSharedRef(),
		FText::FromString("Field of view"),
		Quadcopter->QuadcopterCamera->FieldOfView,
		FOnFloatValueChanged::CreateSP(this, &SQuadcopterSettingsWidget::FieldOfViewChanged)
	);
	NewFloatEntry(
		SettingsBox.ToSharedRef(),
		FText::FromString("Mouse Sensitivity"),
		Quadcopter->MouseSensitivity,
		FOnFloatValueChanged::CreateSP(this, &SQuadcopterSettingsWidget::MouseSensitivityChanged)
	);
}
void SQuadcopterSettingsWidget::NewFloatEntry(TSharedRef<SVerticalBox> SettingsBox, const FText& EntryName, float InitialValue, FOnFloatValueChanged EntryDelegate) {
	SettingsBox->AddSlot()
	.Padding(FMargin(0.f, 0.f, 0.f, 5.f))
	[
		SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			.Padding(FMargin(0.f, 0.f, 5.f, 0.f))
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
					.Text(EntryName)
					.ColorAndOpacity(TextColor.ToSharedRef())
					.Font(SettingsEntryFont)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SBox)
					.MinDesiredWidth(120.f)
					.MinDesiredHeight(40.f)
					[
						SNew(SSpinBox<float>)
							.Value(InitialValue)
							.ContentPadding(FMargin(5.f, 2.f, 1.f, 2.f))
							.Style(SpinboxStyle.Get())
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
							.OnValueChanged(EntryDelegate)
					]
			]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SQuadcopterSettingsWidget::CameraAngleChanged(float NewValue) {
	Quadcopter->QuadcopterCamera->SetRelativeRotation(FRotator(NewValue, 0.f, 0.f));
}
void SQuadcopterSettingsWidget::FieldOfViewChanged(float NewValue) {
	Quadcopter->QuadcopterCamera->FieldOfView = NewValue;
}
void SQuadcopterSettingsWidget::MouseSensitivityChanged(float NewValue) {
	Quadcopter->MouseSensitivity = NewValue;
}
