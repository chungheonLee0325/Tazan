// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Recover.h"

#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/UI/Widget/PlayerStatusWidget.h"
#include "Tazan/Utilities/LogMacro.h"


void UY_Recover::OnCastEnd()
{
	Super::OnCastEnd();
	m_Caster->IsGroggy = false;

	ABaseMonster* y = Cast<ABaseMonster>(m_Caster);
	if (y)
	{
		y->StatusWidget->UpdateStamina(500.0f,1.0f,500.0f);
	}
}
