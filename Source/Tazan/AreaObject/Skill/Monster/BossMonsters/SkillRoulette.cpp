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
	
	// 거리가 멀면, 원거리 공격 활성화.
	if (dist > LongRange)
	{
		// LOG_SCREEN("원거리 공격 활성화");
		SetSkillWeight(localEntries,EAiSkillType::Long,18.0f);
	}

	//직전 스킬 타입이 거리벌리기이면, 원거리 공격 위주로 사용
	if (PrevSkillType == EAiSkillType::Back)
	{
		// LOG_SCREEN("원거리 공격!");
		SelectSkillByType(localEntries,EAiSkillType::Long,32.0f);
	}

	//직전 스킬 타입이 원거리 공격이면, 원거리 공격 확률 DOWN
	else if (PrevSkillType == EAiSkillType::Long)
	{
		ApplySkillWeight(localEntries,EAiSkillType::Long,0.25f);
	}

	//직전 스킬 타입이 좌/우 공격이면, 턴어택 거의 확정 시전
	else if (PrevSkillType == EAiSkillType::Left || PrevSkillType == EAiSkillType::Right)
	{
		SetSkillWeightByID(localEntries,10200,18.0f);
	}
	
	// 거리가 짧으면, 위빙 스킬 확률 UP
	else if (dist <= ShortRange)
	{
		ApplySkillWeight(localEntries,EAiSkillType::Weaving,2.0f);
	}

	float forwardDot = FVector::DotProduct(dir,Owner->GetActorForwardVector());
	float rightDot = FVector::DotProduct(dir,Owner->GetActorRightVector());

	// 플레이어가 뒤에 있다면, 뒤도는 스킬 확률 UP
	if (forwardDot < -0.25f)
	{
		SetSkillWeightByID(localEntries,10800,18.0f);
		SetSkillWeightByID(localEntries,15200,0.0f);
		if (dist <= ShortRange)
		{
			ApplySkillWeightByID(localEntries,10200,12.0f);
		}
	}
		
	if (forwardDot > 0.0f && rightDot >= 0.15f)
	{
		// LOG_PRINT(TEXT("우측 대각선!"));
		if ((PrevSkillType != EAiSkillType::Right) && (PrevSkillType != EAiSkillType::Left))
		{
			SetSkillWeight(localEntries,EAiSkillType::Right,3.0f);
		}
	}
	if (forwardDot > 0.0f && rightDot <= 0.15f)
	{
		// LOG_PRINT(TEXT("좌측 대각선!"));
		if ((PrevSkillType != EAiSkillType::Right) && (PrevSkillType != EAiSkillType::Left))
		{
			SetSkillWeight(localEntries,EAiSkillType::Left,3.0f);
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

void USkillRoulette::ApplySkillWeightByID(TArray<FSkillRouletteEntry>& entries, const int skillID,
	const float ratio) const
{
	for (FSkillRouletteEntry& entry : entries)
	{
		if (entry.SkillID == skillID)
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

void USkillRoulette::SelectSkillByType(TArray<FSkillRouletteEntry>& entries, const EAiSkillType& skillType) const
{
	for (FSkillRouletteEntry& entry : entries)
	{
		if (entry.SkillType != skillType)
		{
			entry.Weight = 0.0f;
		}
	}
}

void USkillRoulette::SelectSkillByType(TArray<FSkillRouletteEntry>& entries, const EAiSkillType& skillType,
	const float weight) const
{
	for (FSkillRouletteEntry& entry : entries)
	{
		if (entry.SkillType == skillType)
		{
			entry.Weight = weight;
		}
		else
		{
			entry.Weight = 1.0f;
		}
	}
}
