// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_StandOff.h"

#include "Chaos/Utilities.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"

void UY_StandOff::InitState()
{
}

void UY_StandOff::Enter()
{
	Yetuga = Cast<AYetuga>(m_Owner);
	m_NextState = EAiStateType::Attack;
	AnimMontagePlay(Yetuga,Yetuga->GetAnimMontage(EYetugaAnimType::Roar));
	//TODO: 플레이어가 탈진 상태인가?
	if (0) 
	{
		//강한 공격
		return;
	}
	//플레이어가 정면이라면
	if (Yetuga->IsPlayerForward()) 
	{
		//TODO: 플레이어한테 맞고 있나?
		if (Yetuga->bIsHit)
		{
			AnimMontagePlay(Yetuga,Yetuga->GetAnimMontage(EYetugaAnimType::BackMove));
		}
		else
		{
			m_AiFSM->ChangeState(EAiStateType::Attack);
		}
	}
	else
	{
		//TODO: 플레이어가 내 뒤인가?
		if (0) 
		{
			//등 공격
		}
		//위치 재정비
	}
}

void UY_StandOff::Execute(float DeltaTime)
{
	//위치 재정비
	//TODO: 플레이어가 날 때리고 있나?
	if (0) 
	{
		//백무브
		return;
	}
	//플레이어의 정면을 보도록
}

void UY_StandOff::Exit()
{
	LOG_SCREEN("대기 종료");
}

