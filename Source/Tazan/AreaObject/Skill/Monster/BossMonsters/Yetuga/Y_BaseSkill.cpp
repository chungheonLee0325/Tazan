// Fill out your copyright notice in the Description page of Project Settings.

#include "Y_BaseSkill.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"

void UY_BaseSkill::OnCastEnd()
{
	// Casting Phase일때 한번만 처리
	if (m_CurrentPhase != ESkillPhase::Casting) return;
	if (!m_Caster || !m_Target) return;

	m_Caster->ClearCurrentSkill();
	if (nullptr != m_NextSkill && m_Caster->CanCastNextSkill(m_NextSkill, m_Target))
	{
		m_NextSkill->OnSkillComplete = OnSkillComplete;
		m_Caster->CastSkill(m_NextSkill, m_Target);
	}
	else
	{
		if (OnSkillComplete.IsBound() == true)
		{
			OnSkillComplete.Execute();
			OnSkillComplete.Unbind();
		}
	}
	m_CurrentPhase = ESkillPhase::CoolTime;
	AdjustCoolTime();
	ABaseMonster* monster = Cast<ABaseMonster>(m_Caster);
	if (monster)
	{
		monster->GetFSM()->ChangeState(EAiStateType::Wait);
	}
}
