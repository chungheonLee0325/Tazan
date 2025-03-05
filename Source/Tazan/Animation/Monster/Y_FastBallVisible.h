// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Y_FastBallVisible.generated.h"

class AYetuga;
/**
 * 
 */
UCLASS()
class TAZAN_API UY_FastBallVisible : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	AYetuga* Yetuga;
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
