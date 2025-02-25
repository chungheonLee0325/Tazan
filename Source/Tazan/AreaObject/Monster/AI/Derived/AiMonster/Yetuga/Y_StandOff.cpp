// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_StandOff.h"

#include "Kismet/KismetMathLibrary.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"

void UY_StandOff::InitState()
{
}

void UY_StandOff::Enter()
{
	LOG_PRINT(TEXT(""));
}

void UY_StandOff::Execute(float DeltaTime)
{
	CurTime += DeltaTime;
	FVector dir = m_Owner->GetDirToTarget();
	dir.Normalize();
	FRotator rot = UKismetMathLibrary::MakeRotFromZX(FVector::UpVector,dir);
	
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

