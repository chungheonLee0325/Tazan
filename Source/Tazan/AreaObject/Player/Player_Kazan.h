// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Player_Kazan.generated.h"

UCLASS()
class TAZAN_API APlayer_Kazan : public AAreaObject
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayer_Kazan();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
