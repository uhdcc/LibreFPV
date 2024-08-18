#include "LibreFPVEditorModule.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "EditorLevelLibrary.h"
#include "Framework/Commands/GenericCommands.h"

IMPLEMENT_MODULE(FLibreFPVEditorModule, LibreFPVEditor);

void FLibreFPVEditorModule::StartupModule() {
	//FEditorDelegates::OnDuplicateActorsBegin.AddRaw(this, &FLibreFPVEditorModule::OnDuplicateActorsBegin);
	//FEditorDelegates::OnEditPasteActorsBegin .AddRaw(this, &FLibreFPVEditorModule::OnEditPasteActorsBegin);
	//FEditorDelegates::OnEditCopyActorsBegin .AddRaw(this, &FLibreFPVEditorModule::OnEditCopyActorsBegin);

}
void FLibreFPVEditorModule::ShutdownModule() {
}
void FLibreFPVEditorModule::OnDuplicateActorsBegin() {
	if (!IsInGameThread() || !GIsEditor || GEditor->PlayWorld || GIsPlayInEditorWorld) {
		return;
	}
	TArray<AActor*> Result;
	for (FSelectionIterator Iter(*GEditor->GetSelectedActors()); Iter; ++Iter) {
		AActor* Actor = Cast<AActor>(*Iter);
		if (Actor && !Actor->IsPendingKill()) {
			UWorld* World = Actor->GetWorld();
			if (World && World->WorldType == EWorldType::Editor) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, LexToString(Actor->GetName()));
				Result.Add(Actor);
			}
		}
	}
}
void FLibreFPVEditorModule::OnEditPasteActorsBegin() {
	if (!IsInGameThread() || !GIsEditor || GEditor->PlayWorld || GIsPlayInEditorWorld) {
		return;
	}
	TArray<AActor*> Result;
	for (FSelectionIterator Iter(*GEditor->GetSelectedActors()); Iter; ++Iter) {
		AActor* Actor = Cast<AActor>(*Iter);
		if (Actor && !Actor->IsPendingKill()) {
			UWorld* World = Actor->GetWorld();
			if (World && World->WorldType == EWorldType::Editor) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, LexToString(Actor->GetName()));
				Result.Add(Actor);
			}
		}
	}
}

void FLibreFPVEditorModule::OnEditCopyActorsBegin() {
	if (!IsInGameThread() || !GIsEditor || GEditor->PlayWorld || GIsPlayInEditorWorld) {
		return;
	}
	TArray<AActor*> Result;
	for (FSelectionIterator Iter(*GEditor->GetSelectedActors()); Iter; ++Iter) {
		AActor* Actor = Cast<AActor>(*Iter);
		if (Actor && !Actor->IsPendingKill()) {
			UWorld* World = Actor->GetWorld();
			if (World && World->WorldType == EWorldType::Editor) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LexToString(Actor->GetName()));
				Result.Add(Actor);
			}
		}
	}
}
