// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_SelectSkill.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Y_SkillRoulette.h"

void UY_SelectSkill::InitState()
{
}

void UY_SelectSkill::Enter()
{
	//LOG_PRINT(TEXT(""));
	
	Dist = m_Owner->GetDistanceTo(m_Owner->GetAggroTarget());
	FVector dir = m_Owner->GetAggroTarget()->GetActorLocation()-m_Owner->GetActorLocation();
	dir.Normalize();
	float forwardDot = FVector::DotProduct(dir,m_Owner->GetActorForwardVector());
	
	// LOG_PRINT(TEXT("포워드 닷: %f"),forwardDot);

	m_NextState = EAiStateType::Chase;
	
	// 플레이어가 예투가 뒤에 있으면 백어택
	if (forwardDot > -1.0f && forwardDot < -0.75f) 
	{
		//LOG_SCREEN("내 뒤");
		if (Dist < WeavingMaxRange)
		{
			if (m_Owner->GetSkillByID(10900)->GetCurrentPhase() == ESkillPhase::Ready)
			{
				m_Owner->NextSkill = m_Owner->GetSkillByID(10900);
				m_NextState = EAiStateType::Attack;
				return;
			}
		}
	}
	
	int id = SkillRoulette->GetRandomSkillID();
	m_Owner->NextSkill = m_Owner->GetSkillByID(id);
}

void UY_SelectSkill::Execute(float DeltaTime)
{
	m_AiFSM->ChangeState(m_NextState);
	return;
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
