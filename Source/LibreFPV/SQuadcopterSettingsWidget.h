// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/SCompoundWidget.h"
#include "Quadcopter.h"

class AQuadcopter;

class LIBREFPV_API SQuadcopterSettingsWidget : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SQuadcopterSettingsWidget)
		: _Quadcopter(nullptr)

	{}
		SLATE_ARGUMENT(TWeakObjectPtr<AQuadcopter>, Quadcopter)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void NewFloatEntry(TSharedRef<SVerticalBox> SettingsBox, const FText& EntryName, float InitialValue, FOnFloatValueChanged EntryDelegate);

	TSharedPtr<FLinearColor> BackgroundColor;
	TSharedPtr<FLinearColor> TextColor;
	FSlateFontInfo SettingsEntryFont;
	TSharedPtr<FSpinBoxStyle> SpinboxStyle;

	void CameraAngleChanged(float NewValue);
	void FieldOfViewChanged(float NewValue);
	void MouseSensitivityChanged(float NewValue);
	TWeakObjectPtr<AQuadcopter> Quadcopter;

};
