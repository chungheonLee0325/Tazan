// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "TazanGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TAZAN_API UTazanGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	FAreaObjectData* GetDataAreaObject(int AreaObjectID);
	FSkill_Data* GetDataSkill(int SkillID);

	TMap<int32, FAreaObjectData> dt_AreaObject;
	TMap<int32, FSkill_Data> dt_Skill;

	UPROPERTY(EditAnywhere)
	TMap<int, USoundBase*> SoundDataMap;

protected:
	virtual void Init() override;
};
