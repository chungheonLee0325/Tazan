// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckbHitComboSkill.h"

#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/AreaObject/Skill/CollisionSkill.h"

void UCheckbHitComboSkill::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		AAreaObject* caster = Cast<AAreaObject>(MeshComp->GetOwner());
		if (caster != nullptr)
		{
			UBaseSkill* skill = caster->GetCurrentSkill();
			if (skill != nullptr)
			{
				UCollisionSkill* collisionSkill = Cast<UCollisionSkill>(skill);
				if (collisionSkill != nullptr)
				{
					collisionSkill->ResetNextSkillByBHit();
				}
			}
		}
	}
}
