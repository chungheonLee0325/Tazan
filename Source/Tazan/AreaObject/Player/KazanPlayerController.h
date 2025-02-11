﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player_Kazan.h"
#include "GameFramework/PlayerController.h"
#include "KazanPlayerController.generated.h"

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class TAZAN_API AKazanPlayerController : public APlayerController
{
	GENERATED_BODY()
	virtual void SetupInputComponent() override;

public:
	AKazanPlayerController();

	virtual void BeginPlay() override;

private:
	// Input Action
	/** Called for movement input */
	void OnMove(const FInputActionValue& Value);
	/** Called for looking input */
	void OnLook(const FInputActionValue& Value);
	/** Called for attack input */
	void On_Attack_Common_Pressed(const FInputActionValue& InputActionValue);
	void On_Attack_Strong_Pressed(const FInputActionValue& InputActionValue);
	/** Called for parry input */
	void On_Parry_Pressed(const FInputActionValue& InputActionValue);
	void On_Parry_Released(const FInputActionValue& InputActionValue);
	/** Called for evade input */
	void On_Evade_Pressed(const FInputActionValue& InputActionValue);
	/** Called for run input */
	void On_Run_Pressed(const FInputActionValue& InputActionValue);
	void On_Run_Released(const FInputActionValue& InputActionValue);

	// Input Setting
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Attack_C Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackCAction;

	/** Attack_S Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackSAction;

	/** Parry Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ParryAction;

	/** Evade Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EvadeAction;

	// Owner
	UPROPERTY(VisibleAnywhere)
	APlayer_Kazan* Kazan;
};
