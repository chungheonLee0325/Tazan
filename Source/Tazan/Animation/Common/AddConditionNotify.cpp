// Fill out your copyright notice in the Description page of Project Settings.


#include "AddConditionNotify.h"

#include "Tazan/AreaObject/Base/AreaObject.h"

void UAddConditionNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		AAreaObject* owner = Cast<AAreaObject>(MeshComp->GetOwner());
		if (owner != nullptr && owner->GetCurrentSkill() != nullptr)
		{
			owner->AddCondition(Condition, DurationTime);
		}
	}
}
