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

	// 풀링 시스템을 위한 초기화/리셋 함수
	void Init(USkeletalMeshComponent* Pawn, float FadeOutDuration = 0.5f, float EnableDelay = 0.0f, bool IsDynamicMaterial = false);
	void Reset();

	// 머티리얼 초기화 함수

	
	void SetInitialOpacity(float Opacity) { InitialOpacity = Opacity; }
	void SetGhostColor(const FLinearColor& Color);
	void SetCustomMaterial(UMaterialInterface* NewMaterial);

	// 풀링 시스템에서 상태 확인용
	bool IsSpawned = false;

private:
	UPROPERTY(EditDefaultsOnly)
	class UPoseableMeshComponent* PoseableMesh;

	//UPROPERTY(EditDefaultsOnly)
	//class UMaterialInstance* GhostMaterial;

	UPROPERTY(EditDefaultsOnly)
	TArray<class UMaterialInstanceDynamic*> DynamicMaterials;

	float FadeCountDown;
	float EnableCountDown;
	bool IsEnabled = false;

	UPROPERTY(EditDefaultsOnly, Category = "Ghost Trail")
	UMaterialInterface* GhostMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Ghost Trail")
	FLinearColor GhostColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(EditDefaultsOnly, Category = "Ghost Trail")
	float MinOpacity = 0.2f;
	float FadeOutTime;
	float InitialOpacity = 0.7f;

	void ClearMaterials();
	void UpdateMaterialParameters();
};
