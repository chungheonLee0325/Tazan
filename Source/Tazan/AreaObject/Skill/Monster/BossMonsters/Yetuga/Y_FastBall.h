// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Y_BaseSkill.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Y_FastBall.generated.h"

class AYetuga_RockS;
/**
 * 
 */
UCLASS()
class TAZAN_API UY_FastBall : public UY_BaseSkill
{
	GENERATED_BODY()

public:
	virtual void OnCastFire() override;
	
};
