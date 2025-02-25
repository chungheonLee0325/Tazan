// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillRoulette.h"

#include "Tazan/ResourceManager/KazanGameType.h"
#include "Tazan/Utilities/LogMacro.h"


USkillRoulette::USkillRoulette()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USkillRoulette::BeginPlay()
{
	Super::BeginPlay();
}


void USkillRoulette::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USkillRoulette::InitFromSkillBag(const FSkillBagData* SkillBagData)
{
	SkillEntries.Empty();
	AvailableSkillEntries.Empty();
	if (SkillBagData)
	{
		// SkillBagData->TypeSkillBag는 TMap<EAiSkillType, FSkillBag>
		for (const auto& TypePair : SkillBagData->TypeSkillBag)
		{
			const FSkillBag& Bag = TypePair.Value;
			// FSkillBag의 TMap<int, int>는 각각 <SkillID, Weight>
			for (const auto& SkillPair : Bag.SkillID_Weight)
			{
				int SkillID = SkillPair.Key;
				float Weight = static_cast<float>(SkillPair.Value);
				SkillEntries.Add(FSkillRouletteEntry(SkillID, Weight));
				AvailableSkillEntries.Add(FSkillRouletteEntry(SkillID, Weight));
			}
		}
	}
}

int USkillRoulette::GetRandomSkillID() const
{
	float totalWeight = 0.0f;
	for (const FSkillRouletteEntry& Entry : AvailableSkillEntries)
	{
		totalWeight += Entry.Weight;
	}

	if (totalWeight <= 0.f)
	{
		return 0;
	}

	float RandomValue = FMath::FRandRange(0.f, totalWeight);
	float AccumulatedWeight = 0.0f;
	for (const FSkillRouletteEntry& Entry : AvailableSkillEntries)
	{
		AccumulatedWeight += Entry.Weight;
		if (RandomValue <= AccumulatedWeight)
		{
			LOG_PRINT(TEXT("스킬 ID: %d"),Entry.SkillID);
			return Entry.SkillID;
		}
	}
	return 0;
}

void USkillRoulette::RemoveSkillEntryByID(const int skillID)
{
	AvailableSkillEntries.RemoveAll([=](const FSkillRouletteEntry& Entry)
	{
		return Entry.SkillID == skillID;
	});
}

void USkillRoulette::AddSkillEntryByID(const int skillID)
{
	const FSkillRouletteEntry* skillEntry = SkillEntries.FindByPredicate([skillID](const FSkillRouletteEntry& Entry)
	{
		return Entry.SkillID == skillID;
	});
	
	if (skillEntry && !AvailableSkillEntries.Contains(*skillEntry))
	{
		AvailableSkillEntries.Add(*skillEntry);
	}
}
