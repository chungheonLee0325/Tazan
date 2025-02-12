// Fill out your copyright notice in the Description page of Project Settings.


#include "StandOff.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

void UStandOff::InitState()
{
}

void UStandOff::Enter()
{
	Yetuga = Cast<AYetuga>(m_Owner);
	float dist = m_Owner->GetDistanceTo((GetWorld()->GetFirstPlayerController())->GetPawn());
	if (dist < 300.0f)
	{
		float n = FMath::RandRange(0,1);
		Yetuga->WeaveingAttack(n);
	}
	else
	{
		float n = FMath::RandRange(2,3);
		Yetuga->WeaveingAttack(n);
	}
}

void UStandOff::Execute(float DeltaTime)
{
	CurTime += DeltaTime;
	if (CurTime > AttackDuration)
	{
		m_AiFSM->ChangeState(m_NextState);
	}
}

void UStandOff::Exit()
{
	CurTime = 0.0f;
}
