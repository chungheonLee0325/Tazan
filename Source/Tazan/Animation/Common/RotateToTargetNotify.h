// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RotateToTargetNotify.generated.h"

/**
 * 
 */
UCLASS()

class TAZAN_API URotateToTargetNotify : public UAnimNotify
{
	GENERATED_BODY()
	UFUNCTION()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	// 회전 시간 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotate Setting")
	float DurationTime = 0.0f;
	// 회전 비율 0.0f ~ 1.0f - 0.0f는 회전 X / 1.0f 완전 회전
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotate Setting")
	float RotationRatio = 1.0f;
};
