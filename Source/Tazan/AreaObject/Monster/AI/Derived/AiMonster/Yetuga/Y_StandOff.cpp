// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_StandOff.h"

#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"

void UY_StandOff::InitState()
{
}

void UY_StandOff::Enter()
{
	m_NextState = EAiStateType::Attack;
	
	//TODO: 플레이어가 탈진 상태인가?
	if (0) 
	{
		//강한 공격
		return;
	}
	APawn* p = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	float dist = m_Owner->GetDistanceTo(p);
	FVector dir = p->GetActorLocation()-m_Owner->GetActorLocation();
	dir.Normalize();
	float playerDir = FVector::DotProduct(dir,m_Owner->GetActorForwardVector());
	LOG_SCREEN("플레이어 방향: %f", playerDir);
	if (dist < 400.0f)
	{
		// LOG_SCREEN("거리는 가깝고");
		if (playerDir > 0.7f && playerDir < 1.0f) 
		{
			LOG_SCREEN("정면!");
			//TODO: 플레이어한테 맞고 있나?
			if (0)
			{
				m_AiFSM->ChangeState(m_NextState);
			}
			else
			{
				m_AiFSM->ChangeState(m_NextState);
			}
		}
		else
		{
			if (playerDir > -1.0f && playerDir < -0.3) 
			{
				LOG_SCREEN("등 뒤!");
				return;
			}
			// AnimMontagePlay(Yetuga,Yetuga->GetAnimMontage(EYetugaAnimType::SweapAtk));
			m_AiFSM->ChangeState(EAiStateType::Return);
		}
	}
}

void UY_StandOff::Execute(float DeltaTime)
{
	//방향 재정비
	CurTime += DeltaTime;
	FVector dir = UGameplayStatics::GetPlayerPawn(this,0)->GetActorLocation()-m_Owner->GetActorLocation();
	dir.Normalize();
	FRotator rot = dir.Rotation();
	
	float rotSpeed = 1.0f;
	FRotator smoothRot = FMath::RInterpTo(m_Owner->GetActorRotation(), rot, CurTime, rotSpeed);
	
	m_Owner->SetActorRotation(smoothRot);
	if (m_Owner->GetActorRotation().Equals(rot,5.0f))
	{
		CurTime = 0.0f;
		m_AiFSM->ChangeState(m_NextState);
	}
}

void UY_StandOff::Exit()
{
}

