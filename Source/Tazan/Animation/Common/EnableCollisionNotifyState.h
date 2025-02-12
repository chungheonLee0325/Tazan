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
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                         float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                        float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkillData)
	FSkill_Data SkillData;

	// 디버그 드로잉 옵션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugDraw = false;

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
