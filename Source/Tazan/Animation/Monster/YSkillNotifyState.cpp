

#include "YSkillNotifyState.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_BaseSkill.h"

void UYSkillNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (MeshComp && MeshComp->GetOwner())
	{
		AYetuga* y = Cast<AYetuga>(MeshComp->GetOwner());
		if (y != nullptr)
		{
			ySkill = Cast<UY_BaseSkill>(y->NextSkill);
			if (ySkill != nullptr)
			{
				ySkill->NotifyBegin();
			}
		}
	}
}

void UYSkillNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	
	if(ySkill)
	{
		ySkill->NotifyTick(FrameDeltaTime);
	}
	
}

void UYSkillNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	
	if(ySkill)
	{
		ySkill->NotifyEnd();
	}
}
