// Fill out your copyright notice in the Description page of Project Settings.


#include "TazanGameInstance.h"

#include "GameplayTagsManager.h"
#include "TazanGameMode.h"
#include "K2Node_GetDataTableRow.h"

void UTazanGameInstance::Init()
{
	Super::Init();

	// AreaObject Data
	UDataTable* AreaObjectTable = LoadObject<UDataTable>(
		nullptr, TEXT("/Script/Engine.DataTable'/Game/_BluePrints/Common/Data/db_AreaObject.db_AreaObject'"));
	if (nullptr != AreaObjectTable)
	{
		TArray<FName> RowNames = AreaObjectTable->GetRowNames();

		for (const FName& RowName : RowNames)
		{
			FAreaObjectData* Row = AreaObjectTable->FindRow<FAreaObjectData>(RowName, TEXT(""));
			if (nullptr != Row)
			{
				dt_AreaObject.Add(Row->AreaObjectID, *Row);
			}
		}
	}

	// Sound Data
	UDataTable* SoundTable = LoadObject<UDataTable>(
		nullptr, TEXT("/Script/Engine.DataTable'/Game/_BluePrints/Common/Data/db_Sound.db_Sound'"));
	if (nullptr != SoundTable)
	{
		TArray<FName> RowNames = SoundTable->GetRowNames();

		for (const FName& RowName : RowNames)
		{
			FSoundData* Row = SoundTable->FindRow<FSoundData>(RowName, TEXT(""));
			if (nullptr != Row)
			{
				SoundDataMap.Add(Row->SoundID, Row->Sound);
			}
		}
	}
}

FAreaObjectData* UTazanGameInstance::GetDataAreaObject(const int AreaObjectID)
{
	if (FAreaObjectData* data = dt_AreaObject.Find(AreaObjectID))
	{
		return data;
	}

	return nullptr;
}