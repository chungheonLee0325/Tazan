

#include "Y_SkillRoulette.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"


UY_SkillRoulette::UY_SkillRoulette()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UY_SkillRoulette::BeginPlay()
{
	Super::BeginPlay();
	AYetuga* yetuga = Cast<AYetuga>(GetOwner());
	if (yetuga)
	{
		Skill_IDArray = yetuga->GetSkillInstancesID().Array();
	}
}

void UY_SkillRoulette::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int UY_SkillRoulette::GetAllRandom() const
{
	if (Skill_IDArray.Num() == 0)
	{
		LOG_SCREEN_MY(4.0f,FColor::Red,"예투가 스킬 목록이 비엇어요!!");
		LOG_PRINT(TEXT("Y_SkillRoulette 확인"));
		return 0;
	}

	int randIndex = FMath::RandRange(0,Skill_IDArray.Num()-1);
	return Skill_IDArray[randIndex];
}

int UY_SkillRoulette::GetWeavingSkill() const
{
	return 11000;
}

int UY_SkillRoulette::GetShortSkill() const
{
	return 11000;
}

int UY_SkillRoulette::GetLongSkill() const
{
	return 11000;
}
