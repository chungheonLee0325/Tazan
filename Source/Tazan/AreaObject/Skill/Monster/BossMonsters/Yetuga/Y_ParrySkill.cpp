// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_ParrySkill.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"

void UY_ParrySkill::OnCastStart(class AAreaObject* Caster, AAreaObject* Target)
{
	Super::OnCastStart(Caster, Target);
	
	ABaseMonster* monster = Cast<ABaseMonster>(m_Caster);
	if (monster)
	{
		monster->bIsParrySkill = true;
	}
}

void UY_ParrySkill::OnCastEnd()
{
	Super::OnCastEnd();
	
	ABaseMonster* monster = Cast<ABaseMonster>(m_Caster);
	if (monster)
	{
		monster->bIsParrySkill = false;
		monster->InitParryStack();
	}
}
