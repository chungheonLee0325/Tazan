// Fill out your copyright notice in the Description page of Project Settings.


#include "YetugaAnimInstance.h"

#include "Tazan/Utilities/LogMacro.h"

void UYetugaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (bIsGroggyParry)
	{
		UpdateBlendValue(DeltaSeconds);
	}
}

void UYetugaAnimInstance::UpdateBlendValue(float DeltaSeconds)
{

	BlendValue += (DeltaSeconds / BlendDuration);
	BlendValue = FMath::Clamp(BlendValue, 0.0f, 1.0f);
	
	LOG_SCREEN("%f", BlendValue);
	
	if (BlendValue >= 1.0f)
	{
		bIsGroggyParry = false;
		BlendValue = 0.0f; 
	}
}
