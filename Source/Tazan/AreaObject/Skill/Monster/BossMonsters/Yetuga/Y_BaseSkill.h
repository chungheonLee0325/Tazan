// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Y_BaseSkill.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UY_BaseSkill : public UBaseSkill
{
	GENERATED_BODY()

public:
	//TODO: CanCast에 범위 체크 기능 빼는 것을 고려해볼 것.
	// virtual bool CanCast(class AAreaObject* Caster, const AAreaObject* Target) const;
	void OnCastEnd() override;
};
