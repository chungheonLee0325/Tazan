// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Attack.h"

#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Y_SkillRoulette.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_BaseSkill.h"

void UY_Attack::InitState()
{
}

void UY_Attack::Enter()
{
	//LOG_PRINT(TEXT(""));
	
	bHasFailed = false;

	// LOG_PRINT(TEXT("넥스트스킬: %s"),*m_Owner->NextSkill->GetName());
	UBaseSkill* skill = m_Owner->NextSkill;
	
	if (m_Owner->CanCastSkill(skill,m_Owner->GetAggroTarget()))
	{
		SkillID = skill->GetSkillID();
		
		skill->OnSkillComplete.BindUObject(this,&UY_Attack::OnSkillCompleted);
		
		m_Owner->CastSkill(skill,m_Owner->GetAggroTarget());
		LOG_PRINT(TEXT("%d 스킬 사용"),SkillID);
		m_Owner->RemoveSkillEntryByID(SkillID);
		
	}
	else
	{
		LOG_PRINT(TEXT("스킬 실행 실패"));
		bHasFailed = true;
	}
}

void UY_Attack::Execute(float dt)
{
	if (bHasFailed)
	{
		m_AiFSM->ChangeState(EAiStateType::SelectSkill);
	}
}

void UY_Attack::Exit()
{
}


void UY_Attack::OnSkillCompleted()
{
	if (m_AiFSM) m_AiFSM->ChangeState(EAiStateType::SelectSkill);
}

void UY_Attack::AddSkillEntry(const int id)
{
	// LOG_PRINT(TEXT("%d 스킬 추가"), SkillID);
	// if (m_AiFSM) m_Owner->AddSkillEntryByID(SkillID);
}

