// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SwordAnim.generated.h"



UCLASS()
class TAZAN_API USwordAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class ASwordEnemy* SwordEnemy;

	
};
