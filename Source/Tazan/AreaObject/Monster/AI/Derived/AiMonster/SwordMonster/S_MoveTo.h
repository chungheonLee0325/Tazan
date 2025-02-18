// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "S_MoveTo.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API US_MoveTo : public UBaseAiState
{
	GENERATED_BODY()

public:
	virtual void InitState() override;
	virtual void Enter() override;
	virtual void Execute(float DeltaTime) override;
	virtual void Exit() override;

	UPROPERTY(VisibleAnywhere,category = "S_FSM")
	class APlayer_Kazan* Target;

	UPROPERTY()
	class ASwordEnemy* SwordEnemy;
	
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="S_FSM")
	float CheckRadius = 1000.0f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="MonsterStatus")
	float Speed = 350.f;

	UFUNCTION()
	bool IsPlayerInCheckRadius();


	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="S_FSM")
	float AttackRadius = 99.0f;

	UFUNCTION()
	bool IsPlayerInAttackRadius();

	UFUNCTION()
	void MoveToPlayer();
};
