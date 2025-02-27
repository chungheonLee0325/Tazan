// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Yetuga_RockS.generated.h"

UCLASS()
class TAZAN_API AYetuga_RockS : public AActor
{
	GENERATED_BODY()

public:
	AYetuga_RockS();

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* Root;
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Mesh;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
