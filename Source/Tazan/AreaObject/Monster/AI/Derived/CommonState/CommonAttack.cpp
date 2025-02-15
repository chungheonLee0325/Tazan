// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonAttack.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"

void UCommonAttack::InitState()
{
	//m_AiStateType = EAiStateType::Attack;
}

void UCommonAttack::Enter()
{
	m_IsSkillStarted = false;
}

void UCommonAttack::Execute(float DeltaTime)
{
	if (!m_Owner) return;

	// 타겟 체크
	AActor* Target = m_Owner->GetAggroTarget();
	if (!Target)
	{
		m_AiFSM->ChangeState(EAiStateType::Idle);
		return;
	}
	if (!m_IsSkillStarted)
	{
		// 공격 스킬 시작
	//	if (UBaseSkill* AttackSkill = m_Owner->FindSkillByState(m_AiStateType))
		{
			//if (m_Owner->CanCastSkill(AttackSkill,Target))
			{
				//AttackSkill->OnSkillComplete.BindUObject(this,&UCommonAttack::OnSkillCompleted);
				//m_Owner->CastSkill(AttackSkill,Target);
				m_IsSkillStarted = true;
			}
			//else
			{
				m_AiFSM->ChangeState(EAiStateType::Idle);
			}
		}
		//else
		{
			m_AiFSM->ChangeState(EAiStateType::Idle);
		}
	}
}

void UCommonAttack::Exit()
{
}

void UCommonAttack::OnSkillCompleted()
{
	if (m_AiFSM) m_AiFSM->ChangeState(m_NextState);
}
