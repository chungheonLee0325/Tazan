// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_SelectSkill.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_SkillRoulette.h"

void UY_SelectSkill::InitState()
{
}

void UY_SelectSkill::Enter()
{
	LOG_PRINT(TEXT(""));
	if (SkillRoulette == nullptr) LOG_SCREEN_ERROR(this, "스킬룰렛 없음");
	
	float dist = m_Owner->GetDistToTarget();

	if (dist > 600.0f)
	{
		UBaseSkill* sk = SkillRoulette->GetRandomLongSkill();
		if (sk != nullptr)
		{
			if (CheckRange(dist,sk->GetSkillRange()))
			{
				m_Owner->CastSkill(sk,m_Owner->GetAggroTarget());
				m_AiFSM->ChangeState(EAiStateType::Attack);
				return;
			}
		}
	}
	
	UBaseSkill* sk = SkillRoulette->GetRandomShortSkill();
	if (sk != nullptr)
	{
		if (CheckRange(dist,sk->GetSkillRange()))
		{
			m_Owner->CastSkill(sk,m_Owner->GetAggroTarget());
			m_AiFSM->ChangeState(EAiStateType::Attack);
			return;
		}
	}

	if (sk == nullptr)
	{
		LOG_SCREEN("셀렉트 스킬 실패!");
		return;
	}
	m_Owner->NextSkill = sk;
	m_AiFSM->ChangeState(EAiStateType::Chase);
}

void UY_SelectSkill::Execute(float DeltaTime)
{
}

void UY_SelectSkill::Exit()
{
}

bool UY_SelectSkill::CheckRange(float dist, float range)
{
	return dist <= range;
}
