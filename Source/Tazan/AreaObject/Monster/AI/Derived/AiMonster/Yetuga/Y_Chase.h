// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "Y_Chase.generated.h"


UCLASS()
class TAZAN_API UY_Chase : public UBaseAiState
{
	GENERATED_BODY()
	
public:
	virtual void InitState() override;
	virtual void Enter() override;
	virtual void Execute(float dt) override;
	virtual void Exit() override;

private:
	float SkillRange = 0.0f;
	float RangeMargin = 5.0f;
	float Speed = 500.f;
};
