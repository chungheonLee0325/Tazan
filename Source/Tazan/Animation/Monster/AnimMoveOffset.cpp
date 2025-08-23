// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimMoveOffset.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"

void UAnimMoveOffset::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		Yetuga = Cast<AYetuga>(MeshComp->GetOwner());
		if (Yetuga != nullptr)
		{
			Yetuga->StartAnimMove();
		}
	}
}

void UAnimMoveOffset::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		if (Yetuga != nullptr)
		{
			Yetuga->EndAnimMove();
		}
	}
}
