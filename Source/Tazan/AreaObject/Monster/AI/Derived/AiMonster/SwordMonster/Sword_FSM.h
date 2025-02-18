// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Sword_FSM.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TAZAN_API USword_FSM : public UBaseAiFSM
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USword_FSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	virtual void InitStatePool() override;

	UPROPERTY()
	class USwordAnim* Anim;

	UPROPERTY()
	class ASwordEnemy* SwordEnemy;

	UPROPERTY()
	class APlayer_Kazan* Target;
	
	
	
};
