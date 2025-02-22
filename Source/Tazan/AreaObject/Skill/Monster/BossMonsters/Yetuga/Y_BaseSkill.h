// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Y_BaseSkill.generated.h"

class ABaseMonster;
/**
 * 
 */
UCLASS()
class TAZAN_API UY_BaseSkill : public UBaseSkill
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin();
	virtual void NotifyTick(float dt);
	virtual void NotifyEnd();

	virtual void NotifyTrigger();

	virtual void AttackTick(float dt);
};
