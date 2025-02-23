// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_StandOff.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_SkillRoulette.h"

void UY_StandOff::InitState()
{
}

void UY_StandOff::Enter()
{
	LOG_PRINT(TEXT(""));
	//TODO: 플레이어가 탈진 상태인가?
	bIsWeaving = false;
	
	float dist = m_Owner->GetDistanceTo(m_Owner->GetAggroTarget());
	FVector dir = m_Owner->GetAggroTarget()->GetActorLocation()-m_Owner->GetActorLocation();
	dir.Normalize();
	
	float forwardDot = FVector::DotProduct(dir,m_Owner->GetActorForwardVector());

	//TODO: 위빙스킬 사정거리로 조정
	if (dist < 390.0f)
	{
		//TODO: 플레이어한테 맞고 있는지에 대한 조건 추가
		AYetuga* yetuga = Cast<AYetuga>(m_Owner);
		if (forwardDot > -1.0f && forwardDot < -0.8) 
		{
			if (m_Owner->GetSkillByID(10900)->GetCurrentPhase() == ESkillPhase::Ready)
			{
				m_Owner->NextSkill = m_Owner->GetSkillByID(10900);
				bIsWeaving = true;
				return;
			}
		}
		
		//확률 테스트
		float probability = FMath::FRand()*100.0f;
		if (forwardDot > 0.5f && probability > 75.0f)
		{
			UBaseSkill* sk = SkillRoulette->GetRandomWeavingSkill();
			if (sk)
			{
				// LOG_SCREEN("견제기 실행");
				m_Owner->NextSkill = sk;
				bIsWeaving = true;
			}
		}
	}
}

void UY_StandOff::Execute(float DeltaTime)
{
	if (bIsWeaving)
	{
		m_AiFSM->ChangeState(EAiStateType::Attack);
	}
	
	CurTime += DeltaTime;
	FVector dir = m_Owner->GetDirToTarget();
	dir.Normalize();
	FRotator rot = dir.Rotation();
	
	float rotSpeed = 1.0f;
	FRotator smoothRot = FMath::RInterpTo(m_Owner->GetActorRotation(), rot, CurTime, rotSpeed);
	
	m_Owner->SetActorRotation(smoothRot);
	if (m_Owner->GetActorRotation().Equals(rot,5.0f))
	{
		CurTime = 0.0f;
		m_AiFSM->ChangeState(EAiStateType::SelectSkill);
	}
}

void UY_StandOff::Exit()
{
}

