// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "EnableCollisionNotifyState.generated.h"
class AAreaObject;
/**
 * 
 */
UCLASS()
class TAZAN_API UEnableCollisionNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                         float TotalDuration) override;
	UFUNCTION()
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                        float FrameDeltaTime) override;
	UFUNCTION()
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	// 사용되는 AttackData의 Index값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackData)
	int AttackDataIndex = 0;

	// 사용하는 AttackData
	FAttackData* AttackData;

	// 디버그 드로잉 옵션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugDraw = false;

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
	USkeletalMeshComponent* GetTargetMesh(AAreaObject* TargetAreaObject) const;
	void ProcessHitDetection(AAreaObject* OwnerAreaObject, USkeletalMeshComponent* SourceMesh);

	void DrawDebugHitDetection(const FVector& Start, const FVector& End, const TArray<FHitResult>& HitResults,
	                           const FRotator& SocketRotation) const;

	UPROPERTY()
	TSet<AActor*> HitActors;

	UPROPERTY()
	AAreaObject* m_Owner;
	UPROPERTY()
	USkeletalMeshComponent* OwnerSourceMesh;
};
