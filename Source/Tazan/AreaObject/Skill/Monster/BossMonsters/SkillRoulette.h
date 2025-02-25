// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillRoulette.generated.h"

struct FSkillBagData;

USTRUCT(BlueprintType)
struct FSkillRouletteEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillRoulette")
	int SkillID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillRoulette")
	float Weight;

	FORCEINLINE bool operator==(const FSkillRouletteEntry& Other) const
	{
		return SkillID == Other.SkillID;
	}
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TAZAN_API USkillRoulette : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillRoulette();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FSkillRouletteEntry> SkillEntries;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FSkillRouletteEntry> AvailableSkillEntries;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	void InitFromSkillBag(const FSkillBagData* dt_SkillBag);
	
	UFUNCTION()
	int GetRandomSkillID() const;

	UFUNCTION()
	void RemoveSkillEntryByID(const int skillID);
	UFUNCTION()
	void AddSkillEntryByID(const int skillID);
};
