// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillRoulette.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "Tazan/Utilities/LogMacro.h"


USkillRoulette::USkillRoulette()
{
}

void USkillRoulette::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<ABaseMonster>(GetOwner());
}

void USkillRoulette::InitFromSkillBag(const FSkillBagData* SkillBagData)
{
	SkillEntries.Empty();
	AvailableSkillEntries.Empty();
	if (SkillBagData)
	{
		// SKillBagData는 TypeSkillBag 정보를 가지고 있다.
		// TypeSkillBag은 TMap<EAiSkillType, FSkillBag>
		// EAiSkillType는 Short/Long/Weaveing 등
		// FSkillBag은 TMap<int, float>. 각각 SkillID / Weight;
		for (const auto& TypePair : SkillBagData->TypeSkillBag)
		{
			const FSkillBag& Bag = TypePair.Value;
			for (const auto& SkillPair : Bag.SkillID_Weight)
			{
				int SkillID = SkillPair.Key;
				float Weight = static_cast<float>(SkillPair.Value);
				SkillEntries.Add(FSkillRouletteEntry(TypePair.Key,SkillID, Weight));
				AvailableSkillEntries.Add(FSkillRouletteEntry(TypePair.Key,SkillID, Weight));
			}
		}
	}
}

int USkillRoulette::GetRandomSkillID() const
{
	TArray<FSkillRouletteEntry> localEntries = AvailableSkillEntries;

	float dist = Owner->GetDistToTarget();
	FVector dir = Owner->GetDirToTarget();
	dir.Normalize();

	//직전 스킬 타입이 원거리 공격이면, 원거리 공격 확률 DOWN
	if (PrevSkillType == EAiSkillType::Long)
	{
		// LOG_SCREEN("직전 스킬이 원거리 스킬입니다.");
		ApplySkillWeight(localEntries,EAiSkillType::Long,0.3f);
	}
	
	// 거리가 멀면, 원거리 공격 확률 UP
	if (dist > LongRange)
	{
		// LOG_SCREEN("원거리 공격 확률 UP");
		ApplySkillWeight(localEntries,EAiSkillType::Long,3.0f);
	}
	// 거리가 짧으면, 위빙 스킬 확률 UP
	else if (dist < ShortRange)
	{
		// LOG_SCREEN("위빙 확률 UP");
		ApplySkillWeight(localEntries,EAiSkillType::Weaving,3.0f);

		float forwardDot = FVector::DotProduct(dir,Owner->GetActorForwardVector());
		float rightDot = FVector::DotProduct(dir,Owner->GetActorRightVector());

		if ((PrevSkillType != EAiSkillType::Right) & (PrevSkillType != EAiSkillType::Left))
		{
			if (forwardDot > 0.0f && rightDot >= 0.15f)
			{
				// LOG_SCREEN("우측 대각선!");
				SetSkillWeight(localEntries,EAiSkillType::Right,7.0f);
			}
			if (forwardDot > 0.0f && rightDot <= 0.15f)
			{
				// LOG_SCREEN("좌측 대각선!");
				SetSkillWeight(localEntries,EAiSkillType::Left,7.0f);
			}
		}
	}
	
	
	float totalWeight = 0.0f;
	for (const FSkillRouletteEntry& Entry : localEntries)
	{
		totalWeight += Entry.Weight;
	}

	if (totalWeight <= 0.f)
	{
		return 0;
	}

	float RandomValue = FMath::FRandRange(0.f, totalWeight);
	float AccumulatedWeight = 0.0f;
	for (const FSkillRouletteEntry& Entry : localEntries)
	{
		AccumulatedWeight += Entry.Weight;
		if (RandomValue <= AccumulatedWeight)
		{
			// LOG_PRINT(TEXT("스킬 ID: %d"),Entry.SkillID);

			PrevSkillType = Entry.SkillType;
			
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

void USkillRoulette::ApplySkillWeight(TArray<FSkillRouletteEntry>& entries, const EAiSkillType& skillType, const float ratio) const
{
	for (FSkillRouletteEntry& entry : entries)
	{
		if (entry.SkillType == skillType)
		{
			entry.Weight *= ratio;
		}
	}
}

void USkillRoulette::SetSkillWeight(TArray<FSkillRouletteEntry>& entries, const EAiSkillType& skillType,
	const float weight) const
{
	for (FSkillRouletteEntry& entry : entries)
	{
		if (entry.SkillType == skillType)
		{
			entry.Weight = weight;
		}
	}
}

void USkillRoulette::SetSkillWeightByID(const int skillID, const float weight)
{
	for (FSkillRouletteEntry& entry : AvailableSkillEntries)
	{
		if (entry.SkillID == skillID)
		{
			entry.Weight = weight;
		}
	}
}
