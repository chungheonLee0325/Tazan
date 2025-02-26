

#include "Y_SkillRoulette.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Algo/RandomShuffle.h"


UY_SkillRoulette::UY_SkillRoulette()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UY_SkillRoulette::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<ABaseMonster>(GetOwner());
}

void UY_SkillRoulette::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UY_SkillRoulette::InitSkill()
{
	AYetuga* yetuga = Cast<AYetuga>(GetOwner());
	if (yetuga)
	{
		Skill_IDs = yetuga->GetSkillInstancesID().Array();
	}
}

UBaseSkill* UY_SkillRoulette::GetRandomAllSkill()
{
	while (true)
	{
		int randIndex = FMath::RandRange(0, Skill_IDs.Num() - 1);
		UBaseSkill* skill = Owner->GetSkillByID(Skill_IDs[randIndex]);
		
		if (skill->GetCurrentPhase() == ESkillPhase::Ready)
		{
			return skill;
		}
	}
}

UBaseSkill* UY_SkillRoulette::GetRandomMainSkill()
{
	TArray<int> mainSkill_IDs = {11000,11100,13000,12000,12100};
	while (true)
	{
		int randIndex = FMath::RandRange(0,mainSkill_IDs.Num()-1);
		UBaseSkill* skill = Owner->GetSkillByID(mainSkill_IDs[randIndex]);
		
		if (skill->GetCurrentPhase() == ESkillPhase::Ready)
		{
			return skill;
		}
	}
}

UBaseSkill* UY_SkillRoulette::GetRandSkillByArray(TArray<int> arr) const
{
	if (arr.Num() == 0)
	{
		LOG_SCREEN_ERROR(this,"스킬 배열이 비어잇어.");
		return nullptr;
	}

	Algo::RandomShuffle(arr);
	
	for (int id :arr)
	{
		LOG_PRINT(TEXT("randIndex: %d"), id);
		
		UBaseSkill* skill = Owner->GetSkillByID(id);
		if (skill == nullptr)
		{
			LOG_PRINT(TEXT("해당 ID의 스킬 없음"));
			return nullptr;
		}
		
		if (skill->GetCurrentPhase() == ESkillPhase::Ready)
		{
			LOG_PRINT(TEXT("가져온 스킬: %s"),*skill->GetName());
			return skill;
		}
	}
	LOG_PRINT(TEXT("모든 스킬이 쿨타임 중이에요"));
	return nullptr;
}

TArray<int> UY_SkillRoulette::GetIDWeavingSkill() const
{
	TArray<int> skillArr;
	for (int i=0; i<Skill_IDs.Num(); ++i)
	{
		if (Skill_IDs[i] % 100 == 0 && Skill_IDs[i] < 10900)
		{
			skillArr.Add(Skill_IDs[i]);
		}
	}
	return skillArr;
}

TArray<int> UY_SkillRoulette::GetIDShortSkill() const
{
	// TArray<int> skillArr;
	// for (int i=0; i<Skill_IDs.Num(); ++i)
	// {
	// 	if (Skill_IDs[i] % 1000 == 0 && Skill_IDs[i]  >= 11000 && Skill_IDs[i] < 12000)
	// 	{
	// 		skillArr.Add(Skill_IDs[i]);
	// 	}
	// }
	TArray<int> skillArr = {11000,11100,11200};
	return skillArr;
}

TArray<int> UY_SkillRoulette::GetIDLongSkill() const
{
	// TArray<int> skillArr;
	// for (int i=0; i<Skill_IDs.Num(); ++i)
	// {
	// 	if (Skill_IDs[i] % 1000 == 0 && Skill_IDs[i] >= 12000)
	// 	{
	// 		skillArr.Add(Skill_IDs[i]);
	// 	}
	// }
	TArray<int> skillArr = {12100};
	return skillArr;
}
