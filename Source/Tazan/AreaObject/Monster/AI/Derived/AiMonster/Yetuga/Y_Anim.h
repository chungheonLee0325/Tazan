// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "Y_Anim.generated.h"

class AYetuga;
/**
 * 
 */
UCLASS()
class TAZAN_API UY_Anim : public UBaseAiState
{
	GENERATED_BODY()

public:
	virtual void InitState() override;
	virtual void Enter() override;
	virtual void Execute(float DeltaTime) override;
	virtual void Exit() override;

	UPROPERTY()
	UAnimMontage* AnimMontage;

private:
	UPROPERTY()
	AYetuga* Yetuga;
};
