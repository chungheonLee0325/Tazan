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

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="S_FSM")
	float Speed = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="S_FSM")
	float direction = 0;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
