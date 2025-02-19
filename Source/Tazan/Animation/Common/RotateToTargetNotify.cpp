// Fill out your copyright notice in the Description page of Project Settings.


#include "RotateToTargetNotify.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"

void URotateToTargetNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		ABaseMonster* owner = Cast<ABaseMonster>(MeshComp->GetOwner());
		AAreaObject* target = Cast<AAreaObject>(owner->GetAggroTarget());
		if (owner != nullptr && target != nullptr)
		{
			FVector targetLocation = target->GetActorLocation();
			if (DurationTime != 0)
			{
				owner->LookAtLocation(targetLocation, DurationTime, RotationRatio);
			}
			else
			{
				owner->LookAtLocationDirect(targetLocation);
			}
		}
	}
}
