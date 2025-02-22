// Fill out your copyright notice in the Description page of Project Settings.


#include "YSkillNotify.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_BaseSkill.h"

void UYSkillNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                           const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AYetuga* y = Cast<AYetuga>(MeshComp->GetOwner());
		if (y != nullptr)
		{
			ySkill = Cast<UY_BaseSkill>(y->NextSkill);
			if (ySkill != nullptr)
			{
				ySkill->NotifyTrigger();
			}
		}
	}
}
