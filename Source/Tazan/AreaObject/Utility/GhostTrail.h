// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostTrail.generated.h"


UCLASS()
class TAZAN_API AGhostTrail : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGhostTrail();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Init(USkeletalMeshComponent* Pawn);
	void InitByMaterials(USkeletalMeshComponent* Pawn, const TArray<UMaterialInterface*>& Materials);

private:
	UPROPERTY(EditDefaultsOnly)
	class UPoseableMeshComponent* PoseableMesh;

	//UPROPERTY(EditDefaultsOnly)
	//class UMaterialInstance* GhostMaterial;

	UPROPERTY(EditDefaultsOnly)
	TArray<class UMaterialInstanceDynamic*> DynamicMaterials;

	UPROPERTY(EditDefaultsOnly)
	float FadeOutTime = 0.2f;

	bool IsSpawned = false;
	float FadeCountDown;

	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;
};
