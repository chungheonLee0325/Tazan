// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_ComboSkill.h"

#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/Utilities/LogMacro.h"

UY_ComboSkill::UY_ComboSkill()
{
	MontageBlendTime = 0.3f;
}

void UY_ComboSkill::OnCastStart(class AAreaObject* Caster, AAreaObject* Target)
{
	Super::OnCastStart(Caster, Target);
}

void UY_ComboSkill::OnCastFire()
{
	if (bHasHit)
	{
		m_SkillData->NextSkillID = 11001;
		OnCastEnd();
		return;
	}
	m_SkillData->NextSkillID = 0;
}
