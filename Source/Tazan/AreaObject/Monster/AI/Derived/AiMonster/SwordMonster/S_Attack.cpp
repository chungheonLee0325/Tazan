// Fill out your copyright notice in the Description page of Project Settings.


#include "S_Attack.h"

#include <Tazan/Utilities/LogMacro.h>

#include "Algo/RandomShuffle.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"

void US_Attack::InitState()
{
	
}

void US_Attack::Enter()
{
	LOG_SCREEN("공격");
	LOG_PRINT(TEXT("공격"));
	
	TArray<int> arr = {1000,1005,1010};
	Algo::RandomShuffle(arr);
	
	UBaseSkill* Skill = nullptr;
	for (int idx : arr)
	{
		Skill = m_Owner->GetSkillByID(idx);
		if (m_Owner->CanCastSkill(Skill,m_Owner->GetAggroTarget()))
		{
			m_Owner->CastSkill(Skill,m_Owner->GetAggroTarget());
			m_Owner->GetCurrentSkill()->OnSkillComplete.BindUObject(this ,&US_Attack::OnSkillComplete);
			return;
		}
	}
	
	// if (RandValue < 3)
	// {
	// 	LOG_PRINT(TEXT("공격1"));
	// 	Skill = m_Owner->GetSkillByID(1000);
	// }
	// else if (RandValue < 6)
	// {
	// 	LOG_PRINT(TEXT("공격2"));
	// 	Skill = m_Owner->GetSkillByID(1005);
	// }
	// else
	// {
	// 	LOG_PRINT(TEXT("공격3"));
	// 	Skill = m_Owner->GetSkillByID(1010);
	// }

	if (m_Owner->CanCastSkill(Skill,m_Owner->GetAggroTarget()))
	{
		LOG_PRINT(TEXT("공격4"));
		m_Owner->CastSkill(Skill,m_Owner->GetAggroTarget());
		m_Owner->GetCurrentSkill()->OnSkillComplete.BindUObject(this ,&US_Attack::OnSkillComplete);
	}
}

void US_Attack::Execute(float DeltaTime)
{
	
}

void US_Attack::Exit()
{
	
}

void US_Attack::OnSkillComplete()
{
	LOG_PRINT(TEXT("스킬 완료"));
	m_AiFSM->ChangeState(EAiStateType::Wait);
}
