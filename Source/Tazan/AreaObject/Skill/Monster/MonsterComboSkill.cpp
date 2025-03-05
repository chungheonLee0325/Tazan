// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterComboSkill.h"

void UMonsterComboSkill::OnCastStart(class AAreaObject* Caster, AAreaObject* Target)
{
	Super::OnCastStart(Caster, Target);
}

void UMonsterComboSkill::OnCastFire()
{
	if (bIsHitOnce)
	{
		m_NextSkillID = 11001;
		OnCastEnd();
		return;
	}
	m_NextSkillID = 0;
}
