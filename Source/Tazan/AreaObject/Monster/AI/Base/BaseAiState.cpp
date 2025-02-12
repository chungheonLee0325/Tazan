// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAiState.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "BaseAiFSM.h"


void UBaseAiState::SetAiFSM(UBaseAiFSM* AiFSM)
{
	m_AiFSM = AiFSM;
}

void UBaseAiState::SetAiKind(EAiStateType StateType)
{
	m_AiStateType = StateType;
}

EAiStateType UBaseAiState::AiStateType() const
{
	return m_AiStateType;
}

void UBaseAiState::SetOwner(ABaseMonster* Owner)
{
	m_Owner = Owner;
}

void UBaseAiState::SetNextState(EAiStateType NextState)
{
	m_NextState = NextState;
}

void UBaseAiState::AnimMontagePlay(ABaseMonster* baseMonster, UAnimMontage* montage)
{
	UAnimInstance* animInst = baseMonster->GetMesh()->GetAnimInstance();

	if (animInst && montage)
	{
		// 몽타주 재생
		animInst->Montage_Play(montage);
			
		// 몽타주 종료 delegate 바인딩
		FOnMontageEnded montageEndDelegate;
		montageEndDelegate.BindUObject(this, &UBaseAiState::AnimMontageEnd);
		animInst->Montage_SetEndDelegate(montageEndDelegate, montage);
	}
}

void UBaseAiState::AnimMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	m_AiFSM->ChangeState(m_NextState);
}
