// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_StandOff.h"

#include <Programs/UnrealBuildAccelerator/Core/Public/UbaBase.h>

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
	
	float dist = m_Owner->GetDistanceTo(m_Owner->GetAggroTarget());
	FVector dir = m_Owner->GetAggroTarget()->GetActorLocation()-m_Owner->GetActorLocation();
	dir.Normalize();
	
	float forwardDot = FVector::DotProduct(dir,m_Owner->GetActorForwardVector());
	// LOG_PRINT(TEXT("플레이어 앞뒤: %f"), forwardDot);
	
	if (dist < 400.0f)
	{
		//TODO: 플레이어한테 맞고 있는지에 대한 조건 추가
		AYetuga* yetuga = Cast<AYetuga>(m_Owner);
		if (forwardDot > -1.0f && forwardDot < -0.7) 
		{
			if (m_Owner->GetSkillByID(10400)->GetCurrentPhase() == ESkillPhase::CoolTime)
			{
				LOG_SCREEN("BackAttack Cooldown...");
				return;
			}
			m_Owner->NextSkill = m_Owner->GetSkillByID(10400);
			m_AiFSM->ChangeState(EAiStateType::Attack);
			return;
		}
		
		//확률 테스트
		float probability = FMath::FRand()*100.0f;
		LOG_PRINT(TEXT("확률: %f"), probability);
		if (probability > 75.0f)
		{
			UBaseSkill* sk = SkillRoulette->GetRandomWeavingSkill();
			if (sk)
			{
				LOG_SCREEN("견제기 실행");
				m_Owner->NextSkill = sk;
				m_AiFSM->ChangeState(EAiStateType::Attack);
			}
		}
	}
}

void UY_StandOff::Execute(float DeltaTime)
{
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

