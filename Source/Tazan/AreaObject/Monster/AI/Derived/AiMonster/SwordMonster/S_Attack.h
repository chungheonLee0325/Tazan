// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "S_Attack.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API US_Attack : public UBaseAiState
{
	GENERATED_BODY()

public:
	virtual void InitState() override;
	virtual void Enter() override;
	virtual void Execute(float DeltaTime) override;
	virtual void Exit() override;

	UFUNCTION()
	void OnSkillComplete();

private:
	bool bIsAttack = false;
};

