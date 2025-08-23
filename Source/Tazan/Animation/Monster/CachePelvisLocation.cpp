// Fill out your copyright notice in the Description page of Project Settings.


#include "CachePelvisLocation.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

void UCachePelvisLocation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (MeshComp && MeshComp->GetOwner())
	{
		AYetuga* Yetuga = Cast<AYetuga>(MeshComp->GetOwner());
		if (Yetuga != nullptr)
		{
			Yetuga->ShowRock();
		}
	}
}
