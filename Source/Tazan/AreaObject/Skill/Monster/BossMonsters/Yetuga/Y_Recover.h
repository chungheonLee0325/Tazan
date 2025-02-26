// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Y_Recover.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UY_Recover : public UBaseSkill
{
	GENERATED_BODY()
private:
	bool bIsActive = false;
	
public:
	virtual void OnCastEnd() override;
};
