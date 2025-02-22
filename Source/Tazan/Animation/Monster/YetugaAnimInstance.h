﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/Animation/Common/BaseAnimInstance.h"
#include "YetugaAnimInstance.generated.h"

class AYetuga;
/**
 * 
 */
UCLASS()
class TAZAN_API UYetugaAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	AYetuga* Yetuga;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	float Speed = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	bool bIsGroggy = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	bool bIsGroggyStr = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	bool bIsGroggyParry = false;
};
