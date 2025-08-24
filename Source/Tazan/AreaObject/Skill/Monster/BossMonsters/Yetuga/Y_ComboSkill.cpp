// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_ComboSkill.h"

#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/Utilities/LogMacro.h"

UY_ComboSkill::UY_ComboSkill()
{
	MontageBlendTime = 0.0f;
}

void UY_ComboSkill::OnCastStart(class AAreaObject* Caster, AAreaObject* Target)
{
	Super::OnCastStart(Caster, Target);
}

void UY_ComboSkill::OnCastFire()
{
	if (bIsHitOnce)
	{
		m_NextSkillID = 11001;
		OnCastEnd();
		return;
	}
	m_NextSkillID = 0;
}
