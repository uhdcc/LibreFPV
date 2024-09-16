// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerConfig.h"
#include "Quadcopter.h"
#include "D:\Apps\Epic Games\UE_4.27\Engine\Plugins\AI\UE4ML\Source\UE4ML\Public\4MLJson.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "Misc/DefaultValueHelper.h"

UPlayerConfig::UPlayerConfig() {
}

void UPlayerConfig::BindToPlayer() {
}

void UPlayerConfig::LoadPlayerConfig(AQuadcopter& LocalQuadcopter, APlayerController& LocalPlayerController) {
	auto ConfigFilePath = FPaths::ProjectConfigDir() / (TEXT("GameConfig.txt"));

	TArray<FString> ConfigLines;
	if (!FPaths::FileExists(ConfigFilePath)) {
		ConfigLines.Add(FString::Printf(TEXT("%s = %f"), *FString("Field of View"), 130.0));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f"), *FString("Camera Angle"), 55.0));
		ConfigLines.Add("");
		ConfigLines.Add(FString::Printf(TEXT("%s = %i"), *FString("Rates Type"), 0));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f, %f, %f"), *FString("RC Rate"), 1.0, 1.0, 1.0));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f, %f, %f"), *FString("Super Rate"), 0.7, 0.7, 0.7));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f, %f, %f"), *FString("RC Expo"), 0.0, 0.0, 0.0));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f"), *FString("Thottle Mid"), 0.5));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f"), *FString("Throttle Expo"), 0.0));
		ConfigLines.Add("");
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Restart Run"), *FString("GenericUSBController_Button5")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("-Throttle"), *FString("GenericUSBController_Axis3")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Pitch"), *FString("GenericUSBController_Axis1")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Yaw"), *FString("GenericUSBController_Axis4")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Roll"), *FString("GenericUSBController_Axis2")));
		ConfigLines.Add("");
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("MousePitch"), *FString("MouseY")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("MouseRoll"), *FString("MouseX")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %f"), *FString("Mouse Sensitivity"), 0.15));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Escape Menu"), *FString("Escape")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Restart Run"), *FString("SpaceBar")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("Throttle"), *FString("W")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("KeyboardYaw"), *FString("D")));
		ConfigLines.Add(FString::Printf(TEXT("%s = %s"), *FString("-KeyboardYaw"), *FString("A")));

		FFileHelper::SaveStringArrayToFile(ConfigLines, *ConfigFilePath);
		ConfigLines.Empty();
	}
	FFileHelper::LoadFileToStringArray(ConfigLines, *ConfigFilePath);
	for (auto& i : ConfigLines) {
		if (!i.Contains("=")) continue;
		i.RemoveSpacesInline();
		i.ToLowerInline();
		auto EqualSignIndex = i.Find("=") + 1;
		auto ValueString = i.RightChop(EqualSignIndex);
		i.LeftInline(EqualSignIndex);
		auto bIsNegative = i.StartsWith("-");
		if (bIsNegative) {
			i.RemoveAt(0);
		}
		else {
			if (i.Equals("fieldofview=")) {
				LocalQuadcopter.QuadcopterCamera->FieldOfView = FCString::Atof(*ValueString);
			}
			else if (i.Equals("cameraangle=")) {
				LocalQuadcopter.QuadcopterCamera->SetRelativeRotation(FRotator(FCString::Atof(*ValueString), 0.f, 0.f));
			}
			else if (i.Equals("ratestype=")) {
				LocalQuadcopter.LegacyRates->currentControlRateProfile.rates_type = FCString::Atoi(*ValueString);
			}
			else if (i.Equals("rcrate=")) {
				TArray<FString> CurrentValues;
				ValueString.ParseIntoArray(CurrentValues, TEXT(","));
				for (size_t i2 = 0; i2 < 3; i2++) {
					LocalQuadcopter.LegacyRates->currentControlRateProfile.rcRates[i2] = FCString::Atof(*CurrentValues[i2]) * 100.f;
				}
			}
			else if (i.Equals("superrate=")) {
				TArray<FString> CurrentValues;
				ValueString.ParseIntoArray(CurrentValues, TEXT(","));
				for (size_t i2 = 0; i2 < 3; i2++) {
					LocalQuadcopter.LegacyRates->currentControlRateProfile.rates[i2] = FCString::Atof(*CurrentValues[i2]) * 100.f;
				}
			}
			else if (i.Equals("rcexpo=")) {
				TArray<FString> CurrentValues;
				ValueString.ParseIntoArray(CurrentValues, TEXT(","));
				for (size_t i2 = 0; i2 < 3; i2++) {
					LocalQuadcopter.LegacyRates->currentControlRateProfile.rcExpo[i2] = FCString::Atof(*CurrentValues[i2]) * 100.f;
				}
			}
			else if (i.Equals("throttlemid=")) {
				LocalQuadcopter.LegacyRates->currentControlRateProfile.thrMid8 = FCString::Atof(*ValueString);
			}
			else if (i.Equals("throttleexpo=")) {
				LocalQuadcopter.LegacyRates->currentControlRateProfile.thrExpo8 = FCString::Atof(*ValueString);
			}
			else if (i.Equals("restartrun=")) {
				LocalPlayerController.PlayerInput->AddActionMapping(FInputActionKeyMapping("RestartRun", *ValueString));
			}
			else if (i.Equals("escapemenu=")) {
				LocalPlayerController.PlayerInput->AddActionMapping(FInputActionKeyMapping("EscapeMenu", *ValueString));
			}
		}
		if (i.Equals("throttle=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("Throttle", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("pitch=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("Pitch", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("yaw=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("Yaw", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("roll=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("Roll", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("pitch=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("KeyboardPitch", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("yaw=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("KeyboardYaw", *ValueString, bIsNegative ? -1.f : 1.f));
		}
		else if (i.Equals("roll=")) {
			LocalPlayerController.PlayerInput->AddAxisMapping(FInputAxisKeyMapping("KeyboardRoll", *ValueString, bIsNegative ? -1.f : 1.f));
		}
	}
	
}
