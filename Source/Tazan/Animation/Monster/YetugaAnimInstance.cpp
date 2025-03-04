// Fill out your copyright notice in the Description page of Project Settings.


#include "YetugaAnimInstance.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"


void UYetugaAnimInstance::NativeUpdateAnimation(float dt)
{
	Super::NativeUpdateAnimation(dt);

	switch (CurrentAnimState)
	{
	case EYAnimState::GroggyProcess:
		GroggyInProgress(dt);
		break;
	case EYAnimState::ChargeGroggy:
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

void UYetugaAnimInstance::ParryEntering(float dt)
{
	BlendValue += (dt / BlendDuration);
	BlendValue = FMath::Clamp(BlendValue, 0.0f, 1.0f);
	if (BlendValue >= 0.99f)
	{
		BlendValue = 0.0f; 
		//LOG_PRINT(TEXT("그로기 프로세스로 변경"));
		CurrentAnimState = EYAnimState::GroggyProcess;
	}
}


void UYetugaAnimInstance::ChargeEntering()
{
	//LOG_PRINT(TEXT("그로기 프로세스로 변경"));
	CurrentAnimState = EYAnimState::GroggyProcess;
}

void UYetugaAnimInstance::GroggyInProgress(float dt)
{
	curTime += dt;
	// LOG_SCREEN("%f",curTime);
	if (curTime >= GroggyDuration)
	{
		curTime = 0.0f;
		//LOG_PRINT(TEXT("그로기엔드"));
		CurrentAnimState = EYAnimState::GroggyEnd;
	}
}

void UYetugaAnimInstance::GroggyEnd()
{
	//LOG_PRINT(TEXT("애니상태:None"));
	CurrentAnimState = EYAnimState::None;
}
