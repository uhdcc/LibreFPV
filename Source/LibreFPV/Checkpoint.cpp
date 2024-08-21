// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "EngineUtils.h"
#include "Components/TextRenderComponent.h"

ACheckpoint::ACheckpoint() {
	auto TextColor = FLinearColor(0.7f, 0.7f, 0.7f, 1.f);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	CheckpointTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckpointTrigger"));
	CheckpointTrigger->SetupAttachment(RootComponent);
	CheckpointTrigger->SetCollisionProfileName("OverlapAllDynamic");

	CheckpointArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("CheckpointArrow"));
	CheckpointArrow->SetupAttachment(CheckpointTrigger);
	CheckpointArrow->ArrowSize = 2.5f;

	CheckpointModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointModel"));
	CheckpointModel->SetupAttachment(RootComponent);

	CheckpointJerseyNumber = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CheckpointJerseyNumber"));
	CheckpointJerseyNumber->SetupAttachment(RootComponent);
	CheckpointJerseyNumber->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	CheckpointJerseyNumber->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
	CheckpointJerseyNumber->WorldSize = 150.f;
	CheckpointJerseyNumber->Text = FText::FromString("99");
	CheckpointJerseyNumber->TextRenderColor = TextColor.ToFColor(false);
	CheckpointJerseyNumber->SetHiddenInGame(true);
	CheckpointJerseyNumber->SetUsingAbsoluteRotation(true);

	CheckpointIndex = -1;
}
void ACheckpoint::BeginPlay() {
	Super::BeginPlay();
	SetCheckpointIndex(CheckpointIndex);
}
void ACheckpoint::PostInitializeComponents() {
	Super::PostInitializeComponents();
}
void ACheckpoint::OnConstruction(const FTransform& Transform) {
	if (auto World = GetWorld()) {
		if (CheckpointIndex == -1) {
			TArray<ACheckpoint*> Checkpoints;
			for (TActorIterator<ACheckpoint> i(World); i; ++i) {
				Checkpoints.Add(*i);
			}
			SetCheckpointIndex(Checkpoints.Num() - 1);
		}
		else {
			ValidateCheckpointIndex();
		}
	}
}
void ACheckpoint::PostRename(UObject* OldOuter, const FName OldName) {
	Super::PostRename(OldOuter, OldName);

}
bool ACheckpoint::Rename(const TCHAR* NewName, UObject* NewOuter, ERenameFlags Flags) {
	auto Output = Super::Rename(NewName, NewOuter, Flags);
	// this is a bugfix for a specific case where you duplicate a checkpoint by holding Alt and dragging the gizmo
	if (auto World = GetWorld()) {
		ACheckpoint* NewCheckpoint = nullptr;
		ACheckpoint* OldCheckpoint = nullptr;

		TArray<ACheckpoint*> Checkpoints;
		for (TActorIterator<ACheckpoint> i(World); i; ++i) {
			Checkpoints.Add(*i);
			if (i->GetName().Contains(FString(NewName))) {
				NewCheckpoint = *i;
			}
			if (i->GetName().Contains(GetName())) {
				OldCheckpoint = *i;
			}
		}
		if (NewCheckpoint && OldCheckpoint) {
			NewCheckpoint->SetCheckpointIndex(Checkpoints.Num() - 1);
		}
	}

	return Output;
}
#if WITH_EDITOR
void ACheckpoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName PropertyName = (PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None);
	// if this checkpoint had it's index changed...
	if ((PropertyName == GET_MEMBER_NAME_STRING_CHECKED(ACheckpoint, CheckpointIndex))) {
		ValidateCheckpointIndex();
		SetCheckpointIndex(CheckpointIndex);
	}
}
bool ACheckpoint::Modify(bool bAlwaysMarkDirty) {

	return Super::Modify(bAlwaysMarkDirty);
}
#endif // WITH_EDITOR

void ACheckpoint::SetCheckpointIndex(int NewIndex) {
	CheckpointJerseyNumber->SetText(FText::AsNumber(NewIndex));
	CheckpointIndex = NewIndex;
}

void ACheckpoint::ValidateCheckpointIndex() {
		if (auto World = GetWorld()) {
			ACheckpoint* TestCheckpoint = nullptr;
			TArray<ACheckpoint*> Checkpoints;
			bool bNeedsFixedNumber = false;
			for (TActorIterator<ACheckpoint> i(World); i; ++i) {
				Checkpoints.Add(*i);
				if (*i != this) {
					// if some other checkpoint has this index already...
					if (i->CheckpointIndex == CheckpointIndex) {
						TestCheckpoint = *i;
						bNeedsFixedNumber = true;
					}
				}
			}
			if (bNeedsFixedNumber) {
				bNeedsFixedNumber = false;
				auto TestNumber = -1;
				Checkpoints.Sort();
				for (auto i2 : Checkpoints) {
					if ((i2->CheckpointIndex - TestNumber) > 1) {
						TestCheckpoint->SetCheckpointIndex(TestNumber + 1);
						bNeedsFixedNumber = true; // found a missing CheckpointIndex
						break;
					}
					TestNumber = i2->CheckpointIndex;
				}
				if (!bNeedsFixedNumber) { // did not find a missing CheckpointIndex
					TestCheckpoint->SetCheckpointIndex(Checkpoints.Num() - 1);
				}
			}
		}
}
