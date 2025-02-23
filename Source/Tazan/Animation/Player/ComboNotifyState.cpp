// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboNotifyState.h"

#include "Tazan/AreaObject/Attribute/CombatComponent.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"

void UComboNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		APlayer_Kazan* owner = Cast<APlayer_Kazan>(MeshComp->GetOwner());
		if (owner != nullptr)
		{
			owner->SetComboState(true, NextComboSkillID);
		}
	}
}

void UComboNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		APlayer_Kazan* owner = Cast<APlayer_Kazan>(MeshComp->GetOwner());
		if (owner != nullptr)
		{
			owner->SetComboState(false, 0);
		}
	}
}
