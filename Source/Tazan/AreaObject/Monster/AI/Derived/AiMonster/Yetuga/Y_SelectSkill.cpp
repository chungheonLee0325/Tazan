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
	// LOG_PRINT(TEXT(""));
	
	if (SkillRoulette == nullptr) LOG_SCREEN_ERROR(this, "스킬룰렛 없음");
	
	m_Owner->UpdateCurrentSkill(SkillRoulette->GetRandomSkill());
	
	float skillRange = m_Owner->GetCurrentSkill()->GetSkillRange()-100.0f;
	AYetuga* yetuaga = Cast<AYetuga>(m_Owner);
	yetuaga->SkillRange = skillRange; 
	
	float dist = m_Owner->GetDistToTarget();
	if (dist > skillRange)
	{
		m_AiFSM->ChangeState(EAiStateType::Chase);
		return;
	}
	m_AiFSM->ChangeState(EAiStateType::Attack);
}

void UY_SelectSkill::Execute(float DeltaTime)
{
}

void UY_SelectSkill::Exit()
{
}

void UY_SelectSkill::SetSkillRoulette(UY_SkillRoulette* skillRoulette)
{
	SkillRoulette = skillRoulette;
}
