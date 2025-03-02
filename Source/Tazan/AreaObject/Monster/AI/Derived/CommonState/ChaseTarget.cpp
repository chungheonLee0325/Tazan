// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaseTarget.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"

void UChaseTarget::InitState()
{
}

void UChaseTarget::CheckState()
{
	if (m_SuccessState == EAiStateType::None) LOG_PRINT(TEXT("Please Set m_SuccessState")); 
	if (m_FailState == EAiStateType::None) LOG_PRINT(TEXT("Please Set m_FailState")); 
}

void UChaseTarget::Enter()
{
	m_SelectedSkillRange = m_Owner->GetNextSkillRange();
	m_CurrentChaseTime = 0.f;
}

void UChaseTarget::Execute(float dt)
{
	if (m_Owner->NextSkill == nullptr)
	{
		LOG_PRINT(TEXT("NextSkill is NULL"));
		ChangeState(m_FailState);
		return;
	}

	// 추적 시간 갱신
	m_CurrentChaseTime += dt;
	// option :: 추적 거리 m_MaxChaseDistance 보다 거리가 멀다면 정해진 상태로 전이
	if (m_MaxChaseDistanceState != EAiStateType::None && m_MaxChaseDistance < m_Owner->GetDistToTarget())
	{
		ChangeState(m_MaxChaseDistanceState);
	}

	// option :: 추적 시간이 m_ChaseExtendedTime 초과 하면 정해진 상태로 전이
	if (m_ChaseExtendedState != EAiStateType::None && m_ChaseExtendedTime < m_CurrentChaseTime)
	{
		ChangeState(m_ChaseExtendedState);
	}

	// 일반 추적 로직
	FVector dir = m_Owner->GetDirToTarget();
	dir.Normalize();
	m_Owner->AddMovementInput(dir * m_ChaseAccelSpeed,1.0f);
	
	float dist = m_Owner->GetDistToTarget();
	if (dist < m_SelectedSkillRange - m_SkillRangeMargin)
	{
		ChangeState(m_SuccessState);
	}
}

void UChaseTarget::Exit()
{
}
