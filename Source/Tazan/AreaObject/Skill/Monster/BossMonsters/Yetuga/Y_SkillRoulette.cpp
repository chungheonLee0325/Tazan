

#include "Y_SkillRoulette.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"


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
		Skill_IDArray = yetuga->GetSkillInstancesID().Array();
	}
}

UBaseSkill* UY_SkillRoulette::GetRandomSkill() const
{
	while (true)
	{
		UBaseSkill* skill = Owner->GetSkillByID(GetIDAllRandom());
		if (skill->GetCurrentPhase() == ESkillPhase::Ready) return skill;
	}
}

int UY_SkillRoulette::GetIDAllRandom() const
{
	if (Skill_IDArray.Num() == 0)
	{
		LOG_SCREEN_ERROR(this,"SkillArray is empty.");
		return 0;
	}
	
	int randIndex = FMath::RandRange(0,Skill_IDArray.Num()-1);
	LOG_PRINT(TEXT("Random ID: %d"), Skill_IDArray[randIndex]);
	return Skill_IDArray[randIndex];
}

int UY_SkillRoulette::GetIDWeavingSkill() const
{
	return 11000;
}

int UY_SkillRoulette::GetIDShortSkill() const
{
	return 11000;
}

int UY_SkillRoulette::GetIDLongSkill() const
{
	return 11000;
}
