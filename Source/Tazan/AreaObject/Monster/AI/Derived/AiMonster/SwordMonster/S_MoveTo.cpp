// Fill out your copyright notice in the Description page of Project Settings.


#include "S_MoveTo.h"
#include <Tazan/Utilities/LogMacro.h>

#include "Algo/RandomShuffle.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/NormalMonsters/SwordEnemy/SwordEnemy.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"

// Overview : 플레이어를 추적하고 싶다


void US_MoveTo::InitState()
{
}

void US_MoveTo::Enter()
{
	LOG_PRINT(TEXT("무브투"));
	if (m_Owner->GetAggroTarget() == nullptr)
	{
		m_Owner->SetAggroTarget(Cast<APlayer_Kazan>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)));
	}
	if (Target == nullptr)
	{
		Target = Cast<APlayer_Kazan>(m_Owner->GetAggroTarget());
	}

	auto selectedSkill = SelectRandomSkillBySkillBag();
	if (selectedSkill == nullptr)
	{
		bIsNoSkill = true;
	}
	else
	{
		bIsNoSkill = false;
		m_Owner->NextSkill = selectedSkill;
		// 스킬 사정거리보다 SKILL_RANGE_DELTA 만큼 더 다가가자
		AttackRadius = selectedSkill->GetSkillRange() - SKILL_RANGE_DELTA;
	}
}

//  나중에  dv - 매틱마다 거리를 재야할까? ?? 1초에 한번은 안되나?

void US_MoveTo::Execute(float DeltaTime)
{
	if (bIsNoSkill)
	{
		m_AiFSM->ChangeState(EAiStateType::Wait);
	}
	if (m_Owner == nullptr)
	{
		return;
	}

	if (bDebug)
	{
		// check radius debug
		DrawDebugSphere(GetWorld(), m_Owner->GetActorLocation(), CheckRadius, 10, FColor::Green, false, 0.5f);
		// attack radius debug
		DrawDebugSphere(GetWorld(), m_Owner->GetActorLocation(), AttackRadius, 10, FColor::Red, false, 0.5f);
		//DrawDebugSphere(GetWorld(), m_Owner->GetActorLocation(), AttackRadius+100, 10, FColor::Cyan, false, 0.5f);
		//DrawDebugSphere(GetWorld(), m_Owner->GetActorLocation(), AttackRadius+200, 10, FColor::Emerald, false, 0.5f);
		//DrawDebugSphere(GetWorld(), m_Owner->GetActorLocation(), AttackRadius+50, 10, FColor::Orange, false, 0.5f);
	}


	LOG_SCREEN("다시 체크해 반경안에있는지");

	if (!IsPlayerInCheckRadius())
	{
		m_AiFSM->ChangeState(EAiStateType::Wait);
		LOG_SCREEN("없다 체크하러가자 ");
		LOG_PRINT(TEXT("웨이트로 상태 변경"));
	}
	else if (IsPlayerInAttackRadius())
	{
		m_AiFSM->ChangeState(EAiStateType::Attack);
		LOG_PRINT(TEXT("공격으로 상태 변경"));
	}
	else
	{
		MoveToPlayer();
	}
}

void US_MoveTo::Exit()
{
}

UBaseSkill* US_MoveTo::SelectRandomSkillBySkillBag()
{
	Algo::RandomShuffle(SkillBag);
	
	UBaseSkill* Skill = nullptr;
	for (int idx : SkillBag)
	{
		Skill = m_Owner->GetSkillByID(idx);
		return Skill;
	}
	return nullptr;
}

bool US_MoveTo::IsPlayerInCheckRadius()
{
	if (m_Owner)
	{
		FVector direction = m_Owner->GetActorLocation() - Target->GetActorLocation();
		float distance = direction.Size();

		return distance < CheckRadius;
	}
	return false;
}

bool US_MoveTo::IsPlayerInAttackRadius()
{
	if (m_Owner)
	{
		FVector direction = m_Owner->GetActorLocation() - Target->GetActorLocation();
		float distance = direction.Size();

		return distance < AttackRadius;
	}
	return false;
}

void US_MoveTo::MoveToPlayer()
{
	if (m_Owner)
	{
		//FVector direction = (m_Owner->GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
		FVector direction = Target->GetActorLocation() - m_Owner->GetActorLocation();;
		m_Owner->AddMovementInput(direction.GetSafeNormal() * Speed);
	}
}
