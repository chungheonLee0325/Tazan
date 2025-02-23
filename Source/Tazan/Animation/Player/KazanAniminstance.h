// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Tazan/Animation/Common/BaseAnimInstance.h"
#include "KazanAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UKazanAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	float speed = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	float direction = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	bool bIsGuard;
	void NativeUpdateAnimation(float DeltaSeconds);
};
