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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotate Setting")
	float DurationTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rotate Setting")
	float RotationRatio = 1.0f;
};
