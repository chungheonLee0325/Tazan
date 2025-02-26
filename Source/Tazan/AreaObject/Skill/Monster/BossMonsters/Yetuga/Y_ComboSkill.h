// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Skill/CollisionSkill.h"
#include "Y_ComboSkill.generated.h"

class ABaseMonster;
/**
 * 
 */
UCLASS()
class TAZAN_API UY_ComboSkill : public UCollisionSkill
{
	GENERATED_BODY()
	
public:
	UY_ComboSkill();
	
private:
	int NextSkillIDField = 0;
	
public:
	virtual void OnCastStart(class AAreaObject* Caster, AAreaObject* Target) override;
	virtual void OnCastFire() override;
};
