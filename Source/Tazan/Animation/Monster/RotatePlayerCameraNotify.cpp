// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatePlayerCameraNotify.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"

void URotatePlayerCameraNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		ABaseMonster* owner = Cast<ABaseMonster>( MeshComp->GetOwner());
		if (owner != nullptr && owner->GetAggroTarget() != nullptr)
		{
			APlayer_Kazan* player = Cast<APlayer_Kazan>( owner->GetAggroTarget() );
			if (player != nullptr)
			{
				FRotator targetRotator = player->GetControlRotation();
				targetRotator.Pitch = Pitch;
				player->HandleCameraRotation(targetRotator, InterpSpeed, owner);
			}
		}
	}
}
