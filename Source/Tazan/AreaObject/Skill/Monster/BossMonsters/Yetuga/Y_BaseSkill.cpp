// Fill out your copyright notice in the Description page of Project Settings.

#include "Y_BaseSkill.h"

#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"


UY_BaseSkill::UY_BaseSkill()
{
	MontageBlendTime = 0.0f;
}

void UY_BaseSkill::OnCastEnd()
{	
	Super::OnCastEnd();
}
