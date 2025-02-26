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
	
	if (PrevSkillType == EAiSkillType::Left)
	{
		// 10200 턴어택
		SetSkillWeightByID(localEntries,10200,22.0f);
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
		ApplySkillWeight(localEntries,EAiSkillType::Weaving,1.5f);

		float forwardDot = FVector::DotProduct(dir,Owner->GetActorForwardVector());
		float rightDot = FVector::DotProduct(dir,Owner->GetActorRightVector());
		
		if (forwardDot > 0.0f && rightDot >= 0.15f)
		{
			LOG_PRINT(TEXT("우측 대각선!"));
			if ((PrevSkillType != EAiSkillType::Right) || (PrevSkillType != EAiSkillType::Left))
			{
				SetSkillWeight(localEntries,EAiSkillType::Right,7.0f);
			}
			else
			{
				LOG_PRINT(TEXT("이전 스킬이 좌우스킬"));
			}
		}
		if (forwardDot > 0.0f && rightDot <= 0.15f)
		{
			LOG_PRINT(TEXT("좌측 대각선!"));
			if ((PrevSkillType != EAiSkillType::Right) || (PrevSkillType != EAiSkillType::Left))
			{
				SetSkillWeight(localEntries,EAiSkillType::Left,7.0f);
			}
			else
			{
				LOG_PRINT(TEXT("이전 스킬이 좌우스킬"));
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

void USkillRoulette::SetSkillWeightByID(TArray<FSkillRouletteEntry>& entries, const int skillID, const float weight) const
{
	for (FSkillRouletteEntry& entry : entries)
	{
		if (entry.SkillID == skillID)
		{
			entry.Weight = weight;
		}
	}
}
