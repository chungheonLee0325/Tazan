// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "SwordSkeleton.generated.h"

UCLASS()
class TAZAN_API ASwordSkeleton : public ABaseMonster
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASwordSkeleton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void HandleGroggy(float Duration) override;
	virtual void HandleStaggerEnd() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UBaseAiFSM* CreateFSM();

	// Weapon Setting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponComponent;
};
