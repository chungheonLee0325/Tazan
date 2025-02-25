// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_SelectSkill.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/SkillRoulette.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_SkillRoulette.h"

void UY_SelectSkill::InitState()
{
}

void UY_SelectSkill::Enter()
{
	LOG_PRINT(TEXT(""));
	float dist = m_Owner->GetDistanceTo(m_Owner->GetAggroTarget());
	FVector dir = m_Owner->GetAggroTarget()->GetActorLocation()-m_Owner->GetActorLocation();
	dir.Normalize();

	int id = SkillRoulette->GetRandomSkillID();
	m_Owner->NextSkill = m_Owner->GetSkillByID(id);
	
	float forwardDot = FVector::DotProduct(dir,m_Owner->GetActorForwardVector());
	
	// // 플레이어가 예투가 뒤에 있으면 백어택
	// if (forwardDot > -1.0f && forwardDot < -0.75f) 
	// {
	// 	if (dist < WeavingMinRange)
	// 	{
	// 		if (m_Owner->GetSkillByID(10900)->GetCurrentPhase() == ESkillPhase::Ready)
	// 		{
	// 			m_Owner->NextSkill = m_Owner->GetSkillByID(10900);
	// 			return;
	// 		}
	// 	}
	// }
	//
	// // 플레이어가 예투가와 너무 가까우면 백무브
	// // TODO: 혹은 거리 벌려서 원거리 공격
	// if (dist < WeavingMinRange)
	// {
	// 	if (m_Owner->GetSkillByID(10700)->GetCurrentPhase() == ESkillPhase::Ready)
	// 	{
	// 		LOG_PRINT(TEXT("백무브 선택"));
	// 		m_Owner->NextSkill = m_Owner->GetSkillByID(10700);
	// 		return;
	// 	}
	// }
	// // 거리가 적당하면 위빙스킬
	// else if (dist < WeavingMaxRange)
	// {
	// 	LOG_PRINT(TEXT("위빙스킬체크"));
	// 	Skill = YSkillRoulette->GetRandomWeavingSkill();
	// 	if (Skill != nullptr)
	// 	{
	// 		m_Owner->NextSkill = Skill;
	// 		LOG_PRINT(TEXT("위빙스킬 선택"));
	// 		return;
	// 	}
	// }
	// // 거리가 너무 멀면 원거리 공격
	// else if (dist >= LongRange)
	// {
	// 	LOG_PRINT(TEXT("원거리 스킬 체크"));
	// 	Skill = YSkillRoulette->GetRandomLongSkill();
	// 	if (Skill != nullptr)
	// 	{
	// 		m_Owner->NextSkill = Skill;
	// 		LOG_PRINT(TEXT("원거리 스킬선택"));
	// 		return;
	// 	}
	// }
	//
	// Skill = YSkillRoulette->GetRandomShortSkill();
	// LOG_PRINT(TEXT("단거리 스킬 체크"));
	// if (Skill != nullptr)
	// {
	// 	m_Owner->NextSkill = Skill;
	// 	LOG_PRINT(TEXT("단거리 스킬 체크"));
	// 	return;
	// }
	// else
	// {
	// 	Skill = m_Owner->GetSkillByID(14000);
	// }
	//
	// // float rightDot = FVector::DotProduct(dir,m_Owner->GetActorRightVector());
	//
	// m_Owner->NextSkill = Skill;
}

void UY_SelectSkill::Execute(float DeltaTime)
{
	LOG_PRINT(TEXT(""));
	m_AiFSM->ChangeState(EAiStateType::Chase);
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
