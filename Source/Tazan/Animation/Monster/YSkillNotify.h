// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "YSkillNotify.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UYSkillNotify : public UAnimNotify
{
	GENERATED_BODY()
private:
	class UY_BaseSkill* ySkill;
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
