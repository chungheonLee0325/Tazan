// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_SelectSkill.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_SkillRoulette.h"

void UY_SelectSkill::InitState()
{
}

void UY_SelectSkill::Enter()
{
	LOG_PRINT(TEXT(""));
	float dist = FVector::Dist(m_Owner->GetAggroTarget()->GetActorLocation(), m_Owner->GetActorLocation());
	
	if (dist > 800.0f)
	{
		Skill = SkillRoulette->GetRandomLongSkill();
		if (Skill == nullptr)
		{
			LOG_PRINT(TEXT("원거리 스킬 모두 쿨"));
			Skill = SkillRoulette->GetRandomShortSkill();
			if (Skill == nullptr)
			{
				LOG_PRINT(TEXT("단거리 스킬 모두 쿨"));
				Skill = SkillRoulette->GetRandomWeavingSkill();
			}
		}
	}
	else
	{
		Skill = SkillRoulette->GetRandomShortSkill();
		if (Skill == nullptr)
		{
			LOG_PRINT(TEXT("단거리 스킬 모두 쿨"));
			Skill = SkillRoulette->GetRandomWeavingSkill();
			if (Skill == nullptr)
			{
				LOG_PRINT(TEXT("모든 스킬 쿨"));
				LOG_SCREEN_ERROR(this,"Select Skill has failed! Something is wrong...");
				Skill = m_Owner->GetSkillByID(14000);
			}
		}
	}
	
	m_Owner->NextSkill = Skill;
}

void UY_SelectSkill::Execute(float DeltaTime)
{
	LOG_PRINT(TEXT(""));
	float dist = FVector::Dist(m_Owner->GetAggroTarget()->GetActorLocation(), m_Owner->GetActorLocation());
	
	if (CheckRange(dist,Skill->GetSkillRange()))
	{
		m_AiFSM->ChangeState(EAiStateType::Attack);
	}
	else
	{
		m_AiFSM->ChangeState(EAiStateType::Chase);
	}
}

void UY_SelectSkill::Exit()
{
}

bool UY_SelectSkill::CheckRange(float dist, float range)
{
	if (dist < range)
	{
		return true;
	}
	return false;
}
