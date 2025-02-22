﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "Y_StandOff.generated.h"

class UY_SkillRoulette;
class AYetuga;

UCLASS()
class TAZAN_API UY_StandOff : public UBaseAiState
{
	GENERATED_BODY()
	
public:
	virtual void InitState() override;
	virtual void Enter() override;
	virtual void Execute(float DeltaTime) override;
	virtual void Exit() override;

	void SetSkillRoulette(UY_SkillRoulette* skillRoulette) { SkillRoulette = skillRoulette; };

private:
	bool bIsWeaving = false; 
	float CurTime;
	
	UY_SkillRoulette* SkillRoulette;
};
