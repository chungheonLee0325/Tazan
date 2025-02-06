// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Tazan/ResourceManager/HarioGameType.h"
#include "Condition.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UCondition : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool IsDead() const { return HasCondition(EConditionType::Dead); }

	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool AddCondition(EConditionType Condition);

	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool RemoveCondition(EConditionType Condition);

	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool HasCondition(EConditionType Condition) const;

	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool ExchangeDead();

	UFUNCTION(BlueprintCallable, Category = "Condition")
	void Restart();

private:
	UPROPERTY()
	uint8 ConditionFlags;
};
