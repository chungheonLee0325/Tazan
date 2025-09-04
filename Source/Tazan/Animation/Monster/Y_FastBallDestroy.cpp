#include "Y_FastBallDestroy.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga_RockS.h"

void UY_FastBallDestroy::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		AYetuga_RockS* rock = Cast<AYetuga_RockS>(MeshComp->GetOwner());
		if (rock != nullptr)
		{
			rock->DoDestroy();
		}
	}
}
