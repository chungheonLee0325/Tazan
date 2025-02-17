// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Y_SkillRoulette.generated.h"

class ABaseMonster;
class UBaseSkill;

USTRUCT(BlueprintType)
struct FSkillRouletteEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SkillRoulette")
	int32 SkillID;

	//스킬 확률 가중치
	UPROPERTY(VisibleAnywhere, Category = "SkillRoulette")
	float Weight;

	FSkillRouletteEntry()
		: SkillID(0)
		, Weight(1.0f)
	{}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TAZAN_API UY_SkillRoulette : public UActorComponent
{
	GENERATED_BODY()

public:
	UY_SkillRoulette();

protected:
	virtual void BeginPlay() override;

private:
	TArray<int> Skill_IDs;
	ABaseMonster* Owner;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void InitSkill();

	UBaseSkill* GetRandomAllSkill();
	UBaseSkill* GetRandomMainSkill();
	UBaseSkill* GetRandomWeavingSkill() const { return GetRandSkillByArray(GetIDWeavingSkill()); }
	UBaseSkill* GetRandomShortSkill() const { return GetRandSkillByArray(GetIDShortSkill()); }
	UBaseSkill* GetRandomLongSkill() const { return GetRandSkillByArray(GetIDLongSkill()); }
	
private:
	UBaseSkill* GetRandSkillByArray(TArray<int> arr) const;
	TArray<int> GetIDWeavingSkill() const;
	TArray<int> GetIDShortSkill() const;
	TArray<int> GetIDLongSkill() const;
};
