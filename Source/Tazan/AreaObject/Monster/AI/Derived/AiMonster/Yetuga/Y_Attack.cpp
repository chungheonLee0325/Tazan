// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Attack.h"

#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

void UY_Attack::InitState()
{
}

void UY_Attack::Enter()
{
	Yetuga = Cast<AYetuga>(m_Owner);
	m_NextState = EAiStateType::Wait;
	AnimMontagePlay(Yetuga,Yetuga->GetAnimMontage(EYetugaAnimType::NormalAtk));
}

void UY_Attack::Execute(float dt)
{
}

void UY_Attack::Exit()
{
}

void UY_Attack::AnimMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	Super::AnimMontageEnd(Montage, bInterrupted);
}


