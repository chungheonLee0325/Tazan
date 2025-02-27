// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HandlePlayerCameraNotify.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UHandlePlayerCameraNotify : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rotate")
	float Pitch = - 20.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rotate")
	float RotateSpeed = 2.f; 
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rotate")
	float PovSize = 100.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rotate")
	float ZoomSpeed = 2.f; 
};
