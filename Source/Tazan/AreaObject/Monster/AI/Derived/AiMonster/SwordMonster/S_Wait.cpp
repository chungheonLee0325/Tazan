// Fill out your copyright notice in the Description page of Project Settings.


#include "S_Wait.h"

#include <Tazan/Utilities/LogMacro.h>
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/NormalMonsters/SwordEnemy/SwordEnemy.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
void US_Wait::InitState()
{
	
}

void US_Wait::Enter()
{
	LOG_SCREEN("기다려");
	
	m_AiFSM->ChangeState(EAiStateType::Attack);


	
}

void US_Wait::Execute(float DeltaTime)
{
	
}

void US_Wait::Exit()
{
	
}

bool US_Wait::ISMoveToPlayer()
{
	FVector dir = SwordEnemy->GetPlayer_Kazan()->GetActorLocation() - SwordEnemy->GetActorLocation();
	dir.Normalize();
	float dot = FVector::DotProduct(dir,SwordEnemy->GetActorForwardVector());
	FVector SetActorLocation(dot,speed);
	if (dot > 0.4)
	{
		return true;
	}
	return false;
}



