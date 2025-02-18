// Fill out your copyright notice in the Description page of Project Settings.


#include "S_Wait.h"
#include <Tazan/Utilities/LogMacro.h>

#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/NormalMonsters/SwordEnemy/SwordEnemy.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"

void US_Wait::InitState()
{
}

void US_Wait::Enter()
{
	// State에서 Target을 저장하는 Case
	//if (Target == nullptr)
	//{
	//	Target = Cast<APlayer_Kazan>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	//}

	// Monster에서 Target 저장하는 Case -> 다른 State에서도 또다시 안얻어와도 되서 좋음
	if (m_Owner->GetAggroTarget() == nullptr)
	{
		m_Owner->SetAggroTarget(Cast<APlayer_Kazan>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)));
	}
	if (Target == nullptr)
	{
		Target = Cast<APlayer_Kazan>(m_Owner->GetAggroTarget());
	}

	LOG_SCREEN("기다리면서 체크해");
	LOG_PRINT(TEXT("기다리면서 체크"));
}

void US_Wait::Execute(float DeltaTime)
{
	if (m_Owner == nullptr)
	{
		return;
	}
	if (IsCheckRadius())
	{
		m_AiFSM->ChangeState(EAiStateType::Idle);
		LOG_SCREEN("범위 안에있다 이동으로가자");
		LOG_PRINT(TEXT("이동으로"));
	}
}

void US_Wait::Exit()
{
	
}


bool US_Wait::IsCheckRadius()
{
	if (m_Owner)
	{
		FVector direction = m_Owner->GetActorLocation() - Target->GetActorLocation();
		float distance = direction.Size();

		return distance <= CheckRadius;
	}
	return false;
}
