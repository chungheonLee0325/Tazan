// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_FastBallVisible.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

void UY_FastBallVisible::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		Yetuga = Cast<AYetuga>(MeshComp->GetOwner());
		if (Yetuga != nullptr)
		{
			Yetuga->ShowRock();
		}
	}
}

void UY_FastBallVisible::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		if (Yetuga != nullptr)
		{
			Yetuga->HideRock();
		}
	}
}
