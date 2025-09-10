// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Y_BaseSkill.h"
#include "Y_ParrySkill.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UY_ParrySkill : public UY_BaseSkill
{
	GENERATED_BODY()

public:
	virtual void OnCastStart(class AAreaObject* Caster, AAreaObject* Target) override;
	virtual void OnCastEnd() override;

protected:
	/*해당 스킬일 때 임시로 패리 스택을 변경합니다.*/
	int SkillParryStackMax = 0;
	int OriginParryStackMax = 0;
};
