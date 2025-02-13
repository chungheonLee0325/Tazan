// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Anim.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

void UY_Anim::InitState()
{
}

void UY_Anim::Enter()
{
	Yetuga = Cast<AYetuga>(m_Owner);
	m_NextState = EAiStateType::Wait;
	UAnimMontage* animMontage = Yetuga->GetAnimMontage(EYetugaAnimType::TurnAtk);
	AnimMontagePlay(Yetuga, animMontage);
}

void UY_Anim::Execute(float DeltaTime)
{
}

void UY_Anim::Exit()
{
}

void UY_Anim::AnimMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	Super::AnimMontageEnd(Montage, bInterrupted);
}
