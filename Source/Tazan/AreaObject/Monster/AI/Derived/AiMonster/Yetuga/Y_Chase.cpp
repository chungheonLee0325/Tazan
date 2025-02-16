// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Chase.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

void UY_Chase::InitState()
{
}

void UY_Chase::Enter()
{
	// LOG_PRINT(TEXT(""));
	SkillRange = Cast<AYetuga>(m_Owner)->SkillRange;
}

void UY_Chase::Execute(float dt)
{
	FVector dir = m_Owner->GetDirToTarget();
	dir.Normalize();
	m_Owner->AddMovementInput(dir,1.0f);
	
	float dist = m_Owner->GetDistToTarget();
	if (dist < SkillRange)
	{
		m_AiFSM->ChangeState(EAiStateType::Attack);
	}
}

void UY_Chase::Exit()
{
}
