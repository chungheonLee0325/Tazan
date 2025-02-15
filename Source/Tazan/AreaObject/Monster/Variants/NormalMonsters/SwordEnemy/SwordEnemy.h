// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "SwordEnemy.generated.h"

UCLASS()
class TAZAN_API ASwordEnemy : public ABaseMonster
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASwordEnemy();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual UBaseAiFSM* CreateFSM() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	

	
}; 


