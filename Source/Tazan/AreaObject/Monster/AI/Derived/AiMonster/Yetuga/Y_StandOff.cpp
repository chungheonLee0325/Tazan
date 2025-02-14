// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_StandOff.h"

#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

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
	
	float forwardDot = FVector::DotProduct(dir,m_Owner->GetActorForwardVector());
	LOG_SCREEN("플레이어 앞뒤: %f", forwardDot);
	
	if (dist < 400.0f)
	{
		//TODO: 플레이어한테 맞고 있는지에 대한 조건 추가
		
		AYetuga* yetuga = Cast<AYetuga>(m_Owner);
		if (forwardDot > -1.0f && forwardDot < -0.7 && !yetuga->bHaveBackAtk) 
		{
			//TODO: 몽타주 플레이 레거시
			AnimMontagePlay(yetuga,yetuga->GetAnimMontage(EYetugaAnimType::BackAtk));
			bIsAnim = true;
			m_NextState = EAiStateType::Wait;
			m_AiFSM->ChangeState(EAiStateType::Return);
			return;
		}
		
		float rightDot = FVector::DotProduct(dir,m_Owner->GetActorRightVector());
		LOG_SCREEN("플레이어 좌우: %f", rightDot);
		
		if (rightDot < 0.5f)
		{
			LOG_SCREEN("우측");
			//TODO: 몽타주 플레이 레거시
			AnimMontagePlay(yetuga,yetuga->GetAnimMontage(EYetugaAnimType::ShortMoveL));
			m_NextState = EAiStateType::Wait;
			m_AiFSM->ChangeState(EAiStateType::Return);
			return;
		}
		else
		{
			LOG_SCREEN("좌측");
			//TODO: 몽타주 플레이 레거시
			AnimMontagePlay(yetuga,yetuga->GetAnimMontage(EYetugaAnimType::ShortMoveR));
			m_NextState = EAiStateType::Wait;
			m_AiFSM->ChangeState(EAiStateType::Return);
			return;
		}
	}
}

void UY_StandOff::Execute(float DeltaTime)
{
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
		m_AiFSM->ChangeState(EAiStateType::Attack);
	}
}

void UY_StandOff::Exit()
{
}

