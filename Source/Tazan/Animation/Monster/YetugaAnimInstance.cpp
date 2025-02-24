// Fill out your copyright notice in the Description page of Project Settings.


#include "YetugaAnimInstance.h"

#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/Utilities/LogMacro.h"

void UYetugaAnimInstance::NativeUpdateAnimation(float dt)
{
	Super::NativeUpdateAnimation(dt);

	switch (CurrentAnimState)
	{
	case EYAnimState::GroggyProcess:
		GroggyInProgress(dt);
		break;
	case EYAnimState::ParryGroggyEnter:
		ParryEntering(dt);
		break;
	case EYAnimState::NormalGroggyEnter:
		break;
	case EYAnimState::GroggyEnd:
		GroggyEnd();
		break;
	default:
		break;
	}
	
}

void UYetugaAnimInstance::GroggyInProgress(float dt)
{
	curTime += dt;
	// LOG_SCREEN("%f",curTime);
	if (curTime >= GroggyDuration)
	{
		BlendValue = 0.0f; 
		curTime = 0.0f;
		CurrentAnimState = EYAnimState::GroggyEnd;
	}
}

void UYetugaAnimInstance::ParryEntering(float dt)
{
	BlendValue += (dt / BlendDuration);
	BlendValue = FMath::Clamp(BlendValue, 0.0f, 1.0f);
}

void UYetugaAnimInstance::GroggyEnd()
{
	Yetuga->ChangeStateToWait();
	CurrentAnimState = EYAnimState::None;
}
