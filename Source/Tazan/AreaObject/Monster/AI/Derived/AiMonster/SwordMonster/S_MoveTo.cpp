// Fill out your copyright notice in the Description page of Project Settings.


#include "S_MoveTo.h"
#include <Tazan/Utilities/LogMacro.h>

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/NormalMonsters/SwordEnemy/SwordEnemy.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"

// Overview : 플레이어를 추적하고 싶다


void US_MoveTo::InitState()
{
}

void US_MoveTo::Enter()
{
	if (m_Owner->GetAggroTarget() == nullptr)
	{
		m_Owner->SetAggroTarget(Cast<APlayer_Kazan>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)));
	}
	if (Target == nullptr)
	{
		Target = Cast<APlayer_Kazan>(m_Owner->GetAggroTarget());
	}
}

//  나중에  dv - 매틱마다 거리를 재야할까? ?? 1초에 한번은 안되나?

void US_MoveTo::Execute(float DeltaTime)
{
	if (m_Owner == nullptr)
	{
		return;
	}

	LOG_SCREEN("다시 체크해 반경안에있는지");

	if (!IsPlayerInCheckRadius())
	{
		m_AiFSM->ChangeState(EAiStateType::Wait);
		LOG_SCREEN("없다 체크하러가자 ");
	}
	else if (IsPlayerInAttackRadius())
	{
		m_AiFSM->ChangeState(EAiStateType::Wait);
		LOG_SCREEN("공격범위안에있다 가자 공격하러");
	}
	else
	{
		MoveToPlayer();
	}
}

void US_MoveTo::Exit()
{
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
		m_Owner->AddMovementInput(direction.GetSafeNormal() *Speed );
		
	}
}