// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void AnimMontagePlay(UAnimMontage* montage);
};
