// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboNotifyState.h"

#include "Tazan/AreaObject/Attribute/CombatComponent.h"

void UComboNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AActor * owner = MeshComp->GetOwner();
	if (!MeshComp || !owner)
	{
		return;
	}
	if (UCombatComponent* Combatcomp = owner->FindComponentByClass<UCombatComponent>())
	{
		Combatcomp->EnableComboState();
	}
}

void UComboNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor * owner = MeshComp->GetOwner();
	if (!MeshComp || !owner)
	{
		return;
	}
	if (UCombatComponent* Combatcomp = owner->FindComponentByClass<UCombatComponent>())
	{
		Combatcomp->ResetComboState();
	}
}

void UComboNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
}
