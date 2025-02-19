// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "Y_SelectSkill.generated.h"

class UY_SkillRoulette;
/**
 * 
 */
UCLASS()
class TAZAN_API UY_SelectSkill : public UBaseAiState
{
	GENERATED_BODY()
public:
	virtual void InitState() override;
	virtual void Enter() override;
	virtual void Execute(float DeltaTime) override;
	virtual void Exit() override;

private:
	UY_SkillRoulette* SkillRoulette;
	UBaseSkill* Skill = nullptr;

public:
	void SetSkillRoulette(UY_SkillRoulette* skillRoulette) { SkillRoulette = skillRoulette; }
	bool CheckRange(float dist, float range);
};
