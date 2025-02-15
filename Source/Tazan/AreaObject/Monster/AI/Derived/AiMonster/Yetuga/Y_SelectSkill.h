// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "Y_SelectSkill.generated.h"

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
};
