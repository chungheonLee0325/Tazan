// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Y_BaseSkill.h"
#include "Y_ChargeAttack.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UY_ChargeAttack : public UY_BaseSkill
{
	GENERATED_BODY()

public:
	UY_ChargeAttack();

	virtual void OnCastStart(class AAreaObject* Caster, AAreaObject* Target) override;
	virtual void CancelCast() override;
	virtual void OnCastTick(float DeltaTime) override;
	
	UFUNCTION()
	void Stun();
};
