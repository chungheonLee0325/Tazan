// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Chase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"

void UY_Chase::InitState()
{
}

void UY_Chase::Enter()
{
	//LOG_PRINT(TEXT(""));
	SkillRange = m_Owner->NextSkill->GetSkillRange();
}

void UY_Chase::Execute(float dt)
{
	if (m_Owner->NextSkill == nullptr)
	{
		LOG_PRINT(TEXT("NextSkill is NULL"));
		return;
	}
	
	FVector dir = m_Owner->GetDirToTarget();
	dir.Normalize();
	m_Owner->AddMovementInput(dir * Speed,1.0f);
	
	float dist = m_Owner->GetDistToTarget();
	if (dist < SkillRange - RangeMargin)
	{
		m_AiFSM->ChangeState(EAiStateType::Attack);
	}
}

void UY_Chase::Exit()
{
}
