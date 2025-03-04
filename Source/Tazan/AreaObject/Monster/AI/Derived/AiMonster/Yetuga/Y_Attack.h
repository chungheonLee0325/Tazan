// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "Y_Attack.generated.h"

class UY_BaseSkill;
/**
 * 
 */
UCLASS()
class TAZAN_API UY_Attack : public UBaseAiState
{
	GENERATED_BODY()

public:
	virtual void InitState() override;
	virtual void Enter() override;
	virtual void Execute(float dt) override;
	virtual void Exit() override;

	UFUNCTION()
	void OnSkillCompleted();
	UFUNCTION()
	void AddSkillEntry(const int id);

private:
	int SkillID = 0;
	bool bHasFailed = false;
};
