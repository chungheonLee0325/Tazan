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
	LOG_SCREEN("어택 종료");
}

void UY_Attack::AnimMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Yetuga->IsPlayerForward())
	{
		m_NextState = EAiStateType::Chase;
	}
	Super::AnimMontageEnd(Montage, bInterrupted);
}


