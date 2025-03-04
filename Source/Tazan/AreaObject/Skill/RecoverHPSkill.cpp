// Fill out your copyright notice in the Description page of Project Settings.


#include "RecoverHPSkill.h"

#include "Tazan/AreaObject/Player/Player_Kazan.h"

void URecoverHPSkill::OnCastFire()
{
	Super::OnCastFire();

	m_Caster->IncreaseHP(m_SkillData->AttackData[0].HealthDamageAmountMax);
	APlayer_Kazan* player = Cast<APlayer_Kazan>(m_Caster);
	if (player)
	{
		player->DecreaseHPRecoverStack();
	}
}
