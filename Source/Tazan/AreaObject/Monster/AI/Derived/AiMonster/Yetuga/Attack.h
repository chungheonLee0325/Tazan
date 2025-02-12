// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "Attack.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UAttack : public UBaseAiState
{
	GENERATED_BODY()

public:
	virtual void InitState() override;
	virtual void Enter() override;
	virtual void Execute(float dt) override;
	virtual void Exit() override;
	virtual void AnimMontageEnd(UAnimMontage* Montage, bool bInterrupted) override;

private:
	UPROPERTY()
	class AYetuga* Yetuga;
};
