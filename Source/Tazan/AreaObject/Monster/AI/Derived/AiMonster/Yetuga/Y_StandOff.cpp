// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_StandOff.h"

#include "Kismet/KismetMathLibrary.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"

void UY_StandOff::InitState()
{
}

void UY_StandOff::Enter()
{
	//LOG_PRINT(TEXT(""));
	// m_Owner->StopRotate();
}

void UY_StandOff::Execute(float DeltaTime)
{
	// LOG_SCREEN("회전중..");
	// 회전
	// CurTime += DeltaTime;
	// FVector dir = m_Owner->GetDirToTarget();
	// dir.Normalize();
	// FRotator rot = UKismetMathLibrary::MakeRotFromZX(FVector::UpVector,dir);
	//
	// float rotSpeed = 3.0f;
	// FRotator smoothRot = FMath::RInterpTo(m_Owner->GetActorRotation(), rot, CurTime, rotSpeed);
	//
	// m_Owner->SetActorRotation(smoothRot);
	// if (m_Owner->GetActorRotation().Equals(rot,10.0f))
	// {
	// 	CurTime = 0.0f;
	// 	bIsRotate = false;
	// 	m_AiFSM->ChangeState(EAiStateType::Chase);
	// }
}

void UY_StandOff::Exit()
{
}

