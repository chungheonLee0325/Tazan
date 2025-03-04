// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Y_FastBall.generated.h"

class AYetuga_RockS;
/**
 * 
 */
UCLASS()
class TAZAN_API UY_FastBall : public UBaseSkill
{
	GENERATED_BODY()
	
public:
	AYetuga_RockS* fastBall;

public:
	virtual void OnCastStart(class AAreaObject* Caster, AAreaObject* Target) override;
	virtual void OnCastFire() override;
};
