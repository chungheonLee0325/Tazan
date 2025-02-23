// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
#include "SetPlayerStateNotify.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API USetPlayerStateNotify : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	public:
	UPROPERTY(EditAnywhere)
	EPlayerState PlayerState = EPlayerState::NORMAL;
};
