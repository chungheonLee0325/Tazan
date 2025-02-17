// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Attack.h"

#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"

void UY_Attack::InitState()
{
}

void UY_Attack::Enter()
{
	// LOG_PRINT(TEXT(""));
	m_NextState = EAiStateType::Wait;
	
	m_Owner->CastSkill(m_Owner->GetCurrentSkill(),m_Owner->GetAggroTarget());
	m_Owner->GetCurrentSkill()->SkillLogPrint();
}

void UY_Attack::Execute(float dt)
{
}

void UY_Attack::Exit()
{
}


