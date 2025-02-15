// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Anim.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

void UY_Anim::InitState()
{
}

void UY_Anim::Enter()
{
	m_NextState = EAiStateType::Wait;
}

void UY_Anim::Execute(float DeltaTime)
{
}

void UY_Anim::Exit()
{
}