// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SwordAnim.generated.h"
//
// UENUM(BlueprintType)
// enum class ESwordState : uint8
// {
// 	Idle = 0,
// 	Move = 1,
// 	Attack = 2,
// 	Die = 3
// };

/**
 * 
 */
UCLASS()
class TAZAN_API USwordAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class ASwordEnemy* SwordEnemy;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = S_FSM)
	// ESwordState SwordState = ESwordState::Idle;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	class USwordAnim* SwordAnim;
	
	
};
