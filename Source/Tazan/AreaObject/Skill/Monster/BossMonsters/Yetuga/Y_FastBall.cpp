// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_FastBall.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

void UY_FastBall::OnCastFire()
{
	AYetuga* yetuga = Cast<AYetuga>(m_Caster);
	
	if (yetuga)
	{
		//FAttackCollision* AttackCollision = NotifyStateMap.Find(AttackDataIndex);
		yetuga->FastBall();
	}
}
