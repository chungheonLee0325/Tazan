// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Attack.h"

#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_BaseSkill.h"

void UY_Attack::InitState()
{
}

void UY_Attack::Enter()
{
	bHasFailed = false;
	bIsYSkill = false;
	m_Owner->InitParryStack();
	
	if (m_Owner->CanCastSkill(m_Owner->NextSkill,m_Owner->GetAggroTarget()))
	{
		m_Owner->NextSkill->OnSkillComplete.BindUObject(this,&UY_Attack::OnSkillCompleted);
		m_Owner->CastSkill(m_Owner->NextSkill,m_Owner->GetAggroTarget());
	}
	else
	{
		LOG_PRINT(TEXT("스킬 실행 실패"));
		bHasFailed = true;
	}
	
	ySkill = Cast<UY_BaseSkill>(m_Owner->GetCurrentSkill());
	if (ySkill)
	{
		bIsYSkill = true;	
	}
}

void UY_Attack::Execute(float dt)
{
	if (bHasFailed)
	{
		m_AiFSM->ChangeState(EAiStateType::SelectSkill);
	}
	else if (bIsYSkill)
	{
		ySkill->AttackTick(dt);
	}
}

void UY_Attack::Exit()
{
}


void UY_Attack::OnSkillCompleted()
{
	if (m_AiFSM) m_AiFSM->ChangeState(m_NextState);
}

