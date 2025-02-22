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
	FSkillData* GetDataSkill(int SkillID);
	FSkillBagData* GetDataSkillBag(int SkillBagID);

	TMap<int32, FAreaObjectData> dt_AreaObject;
	TMap<int32, FSkillData> dt_Skill;
	TMap<int32, FSkillBagData> dt_SkillBag;

	UPROPERTY(EditAnywhere)
	TMap<int, USoundBase*> SoundDataMap;

protected:
	virtual void Init() override;
};
