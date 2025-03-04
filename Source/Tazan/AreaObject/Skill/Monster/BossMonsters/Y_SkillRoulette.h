// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tazan/AreaObject/Monster/BaseSkillRoulette.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "Y_SkillRoulette.generated.h"

class ABaseMonster;
struct FSkillBagData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TAZAN_API UY_SkillRoulette : public UBaseSkillRoulette
{
	GENERATED_BODY()

public:
	UY_SkillRoulette();

	virtual void AdjustSkillWeightsByCombatContext(TArray<FSkillRouletteEntry>& LocalEntries) override;
};
