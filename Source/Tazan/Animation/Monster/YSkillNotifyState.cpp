

#include "YSkillNotifyState.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_BaseSkill.h"

void UYSkillNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     float TotalDuration)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		AYetuga* y = Cast<AYetuga>(MeshComp->GetOwner());
		if (y != nullptr)
		{
			ySkill = Cast<UY_BaseSkill>(y->NextSkill);
			if (ySkill != nullptr)
			{
			}
		}
	}
}

void UYSkillNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime)
{
}

void UYSkillNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
}
