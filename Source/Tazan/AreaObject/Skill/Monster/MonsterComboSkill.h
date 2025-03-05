// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Skill/CollisionSkill.h"
#include "MonsterComboSkill.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UMonsterComboSkill : public UCollisionSkill
{
	GENERATED_BODY()
private:
	int NextSkillIDField = 0;
	
public:
	virtual void OnCastStart(class AAreaObject* Caster, AAreaObject* Target) override;
	virtual void OnCastFire() override;
};
