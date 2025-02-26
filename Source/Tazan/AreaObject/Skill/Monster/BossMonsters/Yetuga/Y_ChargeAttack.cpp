// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_ChargeAttack.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/Utilities/LogMacro.h"

UY_ChargeAttack::UY_ChargeAttack()
{
	MontageBlendTime = 0.3f;
}

void UY_ChargeAttack::OnCastStart(class AAreaObject* Caster, AAreaObject* Target)
{
	Super::OnCastStart(Caster, Target);
	this->OnSkillCancel.BindUObject(this,&UY_ChargeAttack::Stun);
}

void UY_ChargeAttack::CancelCast()
{
	Super::CancelCast();
}

void UY_ChargeAttack::Stun()
{
	AYetuga* yetuga = Cast<AYetuga>(m_Caster);
	if (yetuga)
	{
		yetuga->ChargeSkillStun();
	}
}
