// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "Y_SelectSkill.generated.h"

UCLASS()
class TAZAN_API UY_SelectSkill : public UBaseAiState
{
	GENERATED_BODY()
public:
	virtual void InitState() override;
	virtual void Enter() override;
	virtual void Execute(float DeltaTime) override;
	virtual void Exit() override;

	UPROPERTY(EditAnywhere)
	float WeavingMinRange = 400.0f;
	UPROPERTY(EditAnywhere)
	float WeavingMaxRange = 600.0f;
	UPROPERTY(EditAnywhere)
	float LongRange = 1000.0f;


private:
	UPROPERTY()
	class USkillRoulette* SkillRoulette = nullptr;
	UPROPERTY()
	UBaseSkill* Skill = nullptr;

	bool bIsWeaving = false;

public:
	void SetSkillRoulette(USkillRoulette* skillRoulette) { SkillRoulette = skillRoulette; }
	bool CheckRange(float dist, float range);
};
