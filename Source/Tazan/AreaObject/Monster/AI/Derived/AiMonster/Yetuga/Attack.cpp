// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack.h"

#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

void UAttack::InitState()
{
}

void UAttack::Enter()
{
	Yetuga = Cast<AYetuga>(m_Owner);
	m_NextState = EAiStateType::Wait;
	AnimMontagePlay(Yetuga,Yetuga->GetAnimMontage(EYetugaAnimType::NormalAtk));
}

void UAttack::Execute(float dt)
{
}

void UAttack::Exit()
{
	LOG_SCREEN("어택 종료");
}

void UAttack::AnimMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	Super::AnimMontageEnd(Montage, bInterrupted);
}


