// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Skill/CollisionSkill.h"
#include "Y_BaseSkill.generated.h"

class ABaseMonster;
/**
 * 
 */
UCLASS()
class TAZAN_API UY_BaseSkill : public UCollisionSkill
{
	GENERATED_BODY()
	
public:
	UY_BaseSkill();
};
