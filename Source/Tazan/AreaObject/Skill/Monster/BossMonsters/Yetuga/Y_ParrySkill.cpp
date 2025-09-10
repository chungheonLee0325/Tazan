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
		if (SkillParryStackMax != 0)
		{
			OriginParryStackMax = monster->GetParryStack();
			monster->ChangeParryStackMax(SkillParryStackMax);
			LOG_SCREEN("패리 맥스 변경: %d",SkillParryStackMax);
		}
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

		if (SkillParryStackMax != 0)
		{
			monster->ChangeParryStackMax(OriginParryStackMax);
			LOG_SCREEN("패리 맥스 복원: %d",OriginParryStackMax);
		}
	}
}
