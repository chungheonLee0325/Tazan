// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/BaseSkill.h"
#include "CollisionSkill.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UCollisionSkill : public UBaseSkill
{
	GENERATED_BODY()

public:
	virtual void OnCastEnd() override;
	virtual void CancelCast() override;
	virtual void OnCastTick(float DeltaTime) override;
	void SetCasterMesh(int AttackDataIndex);
	void ProcessHitDetection();
	void ResetCollisionData();


	// 디버그 드로잉 옵션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugDraw = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugData = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (EditCondition = "bDebugDraw"))
	FAttackData DebugAttackData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (EditCondition = "bDebugDraw"))
	float DebugDrawDuration = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (EditCondition = "bDebugDraw"))
	FColor DebugColor = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (EditCondition = "bDebugDraw"))
	bool bPersistentLines = false;

private:
	void DrawDebugHitDetection(const FVector& Start, const FVector& End, const TArray<FHitResult>& HitResults,
	                           const FRotator& SocketRotation) const;
	UPROPERTY()
	USkeletalMeshComponent* OwnerSourceMesh;
	UPROPERTY()
	TSet<AActor*> HitActors;

	bool IsEnableHitDetection;

	FAttackData* IndexedAttackData;
};
