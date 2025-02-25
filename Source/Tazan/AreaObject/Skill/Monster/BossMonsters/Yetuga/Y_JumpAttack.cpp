// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_JumpAttack.h"

#include "Tazan/AreaObject/Base/AreaObject.h"

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
