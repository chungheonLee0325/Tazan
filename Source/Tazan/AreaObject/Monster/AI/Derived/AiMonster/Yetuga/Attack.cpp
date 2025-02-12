// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack.h"

#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"

void UAttack::InitState()
{
}

void UAttack::Enter()
{
	
}

void UAttack::Execute(float dt)
{

	UE_LOG(LogTemp,Display,TEXT("공격!"));
	m_AiFSM->ChangeState(m_NextState);
}

void UAttack::Exit()
{
	
}
