// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

class UBoxComponent;
class UArrowComponent;
class UTextRenderComponent;

UCLASS()
class LIBREFPV_API ACheckpoint : public AActor {
	GENERATED_BODY()	
public:	
	ACheckpoint();
	bool operator<(const ACheckpoint& Other) const {
		return CheckpointIndex < Other.CheckpointIndex;
	}
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostRename(UObject* OldOuter, const FName OldName) override;
	virtual bool Rename(const TCHAR* NewName = nullptr, UObject* NewOuter = nullptr, ERenameFlags Flags = REN_None);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

#endif // WITH_EDITOR

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* CheckpointTrigger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CheckpointModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* CheckpointArrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextRenderComponent* CheckpointJerseyNumber;
	UFUNCTION(BlueprintCallable)
	void SetCheckpointIndex(int NewIndex);
	UFUNCTION(BlueprintCallable)
	int GetCheckpointIndex() { return CheckpointIndex; };
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int CheckpointIndex;
	void ValidateCheckpointIndex();
};
