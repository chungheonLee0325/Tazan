// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Chase.h"

#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"

void UY_Chase::InitState()
{
}

void UY_Chase::Enter()
{
	LOG_SCREEN("추적");
	//TODO: SKillBag?에서 nextSkill의 사정거리 가져오기
}

void UY_Chase::Execute(float dt)
{
	APawn* p = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector dir = p->GetActorLocation()-m_Owner->GetActorLocation();
	dir.Normalize();
	m_Owner->AddMovementInput(dir,1.0f);
	
	float dist = m_Owner->GetDistanceTo(p);
	if (dist < SkillRange)
	{
		m_AiFSM->ChangeState(EAiStateType::Attack);
	}
}

void UY_Chase::Exit()
{
}
