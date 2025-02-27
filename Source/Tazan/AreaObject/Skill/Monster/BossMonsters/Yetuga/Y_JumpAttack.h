// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Y_BaseSkill.h"
#include "Tazan/AreaObject/Skill/CollisionSkill.h"
#include "Y_JumpAttack.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UY_JumpAttack : public UCollisionSkill
{
	GENERATED_BODY()

public:
	UY_JumpAttack();

private:
	FVector StartLoc;
	FVector TargetLoc;

public:
	virtual void OnCastStart(class AAreaObject* Caster, AAreaObject* Target) override;
	virtual void OnCastTick(float DeltaTime) override;
	
	virtual void OnCastFire() override;
	
};
