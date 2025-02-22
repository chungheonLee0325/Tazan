// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Y_BaseSkill.h"
#include "Y_JumpAttack.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UY_JumpAttack : public UY_BaseSkill
{
	GENERATED_BODY()

public:
	float JumpPower = 5.0f;
	float tracePower = 5.0f;
	FVector TargetLoc = FVector::ZeroVector;

public:
	virtual void NotifyBegin() override;
	virtual void NotifyTick(float dt) override;
	virtual void NotifyEnd() override;

	virtual void NotifyTrigger() override;

private:
	FVector GetFlyingLocation();
	FVector GetLandingLocation();
};
