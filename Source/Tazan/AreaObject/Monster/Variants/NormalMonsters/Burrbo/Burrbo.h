// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Burrbo.generated.h"

class USphereComponent;

UCLASS()
class TAZAN_API ABurrbo : public ABaseMonster
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABurrbo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual UBaseAiFSM* CreateFSM() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* BodyCollider;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	float GetDetectRange() const;

private:
	UPROPERTY(EditAnywhere, Category = "Monster Settings")
	float ChaseSpeed = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Monster Settings")
	float ReturnSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Monster Settings")
	float DetectRange = 1500.0f;
};
