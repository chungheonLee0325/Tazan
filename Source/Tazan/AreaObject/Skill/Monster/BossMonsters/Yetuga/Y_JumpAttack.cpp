// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_JumpAttack.h"

#include "Tazan/AreaObject/Base/AreaObject.h"

void UY_JumpAttack::NotifyBegin()
{
	Super::NotifyBegin();
}

void UY_JumpAttack::NotifyTick(float dt)
{
	Super::NotifyTick(dt);
}

void UY_JumpAttack::NotifyEnd()
{
	Super::NotifyEnd();
}

void UY_JumpAttack::NotifyTrigger()
{
	Super::NotifyTrigger();
}

FVector UY_JumpAttack::GetFlyingLocation()
{
	float dist = (m_Target->GetActorLocation() - m_Caster->GetActorLocation()).Size();
	// FVector xyDir = 
	return FVector(0, 0, 0);
}

FVector UY_JumpAttack::GetLandingLocation()
{
	return FVector(0, 0, 0);
}
