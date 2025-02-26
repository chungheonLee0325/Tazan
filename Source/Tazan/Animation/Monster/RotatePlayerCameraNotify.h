// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RotatePlayerCameraNotify.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API URotatePlayerCameraNotify : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rotate")
	float Pitch = - 20.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rotate")
	float InterpSpeed = 2.f; 
};
