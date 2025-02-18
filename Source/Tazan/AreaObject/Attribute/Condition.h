// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Tazan/ResourceManager/KazanGameType.h"
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
	bool IsDead() const { return HasCondition(EConditionBitsType::Dead); }

	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool AddCondition(EConditionBitsType Condition, float Duration = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool RemoveCondition(EConditionBitsType Condition);

	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool HasCondition(EConditionBitsType Condition) const;

	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool ExchangeDead();

	UFUNCTION(BlueprintCallable, Category = "Condition")
	void Restart();

private:
	UPROPERTY()
	uint32 ConditionFlags;

	// Condition별 타이머 핸들 관리
	UPROPERTY()
	TMap<EConditionBitsType, FTimerHandle> ConditionTimers;
	

	bool _addCondition(EConditionBitsType Condition);
	bool _removeCondition(EConditionBitsType Condition);
	void RemoveConditionInternal(EConditionBitsType Condition);
};
