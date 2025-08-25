// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/BaseSkill.h"
#include "CollisionSkill.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAttackCollision
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* OwnerSourceMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<AActor*> HitActors = {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsEnableHitDetection = false;
	FAttackData* IndexedAttackData = nullptr;

	// 이전 프레임의 시작/끝 위치 저장
	FVector PreviousStartLocation = FVector::ZeroVector;
	FVector PreviousEndLocation = FVector::ZeroVector;
	FRotator PreviousSocketRotation = FRotator::ZeroRotator;
	bool bHasPreviousPositions = false;
};


UCLASS()
class TAZAN_API UCollisionSkill : public UBaseSkill
{
	GENERATED_BODY()

public:
	virtual void OnCastEnd() override;
	virtual void CancelCast() override;
	virtual void OnCastTick(float DeltaTime) override;
	void SetCasterMesh(int AttackDataIndex);
	void ProcessHitDetection(int AttackDataIndex);
	void ResetCollisionData(int AttackDataIndex);
	bool PerformCollisionCheck(
		EHitDetectionType DetectionType,
		const FVector& StartLocation,
		const FVector& EndLocation,
		const FRotator& SocketRotation,
		const FHitBoxData& HitBoxData,
		const FCollisionQueryParams& QueryParams,
		TArray<FHitResult>& OutHitResults);
	virtual void ResetNextSkillByBHit() override;
	
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

protected:
	bool bIsHitOnce = false;

private:
	void DrawDebugHitDetection(int AttackDataIndex, const FVector& Start, const FVector& End,
	                           const TArray<FHitResult>& HitResults,
	                           const FRotator& SocketRotation);

	UPROPERTY()
	// ToDo : UAnimNotifyState 대신 int(AttackIndex)를 써도 될듯함
	TMap<int, FAttackCollision> NotifyStateMap;
};
