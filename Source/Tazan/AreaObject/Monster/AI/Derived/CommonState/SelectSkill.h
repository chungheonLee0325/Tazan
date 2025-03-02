// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "SelectSkill.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API USelectSkill : public UBaseAiState
{
	GENERATED_BODY()

public:
	virtual void InitState() override;
	virtual void CheckState() override;
	virtual void Enter() override;
	virtual void Execute(float DeltaTime) override;
	virtual void Exit() override;

	void SetSkillRoulette(class UY_SkillRoulette* skillRoulette) { SkillRoulette = skillRoulette; }
	bool CheckRange(float dist, float range);

private:
	UPROPERTY()
	class UY_SkillRoulette* SkillRoulette = nullptr;
	UPROPERTY()
	UBaseSkill* Skill = nullptr;

	float Dist = 0.0f;
	bool bIsWeaving = false;
	
	UPROPERTY(EditAnywhere)
	float LongRange = 1000.0f;
};
