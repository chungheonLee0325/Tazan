// Fill out your copyright notice in the Description page of Project Settings.


#include "KazanPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player_Kazan.h"

AKazanPlayerController::AKazanPlayerController()
{
	// Enhanced Input Setting
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> tempInputMapping(
		TEXT("/Script/EnhancedInput.InputMappingContext'/Game/_Input/IMC_Kazan.IMC_Kazan'"));
	if (tempInputMapping.Succeeded())
	{
		DefaultMappingContext = tempInputMapping.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempMoveAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanMove.IA_KazanMove'"));
	if (tempMoveAction.Succeeded())
	{
		MoveAction = tempMoveAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempLookAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanLook.IA_KazanLook'"));
	if (tempLookAction.Succeeded())
	{
		LookAction = tempLookAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempAttackCAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanAttack_C.IA_KazanAttack_C'"));
	if (tempAttackCAction.Succeeded())
	{
		AttackCAction = tempAttackCAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempAttackSAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanAttack_S.IA_KazanAttack_S'"));
	if (tempAttackSAction.Succeeded())
	{
		AttackSAction = tempAttackSAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempParryAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanParry.IA_KazanParry'"));
	if (tempParryAction.Succeeded())
	{
		ParryAction = tempParryAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempEvadeAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanEvade.IA_KazanEvade'"));
	if (tempEvadeAction.Succeeded())
	{
		EvadeAction = tempEvadeAction.Object;
	}
	
	Kazan = nullptr;
}

void AKazanPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		this->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	Kazan = Cast<APlayer_Kazan>(GetPawn());
}

void AKazanPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AKazanPlayerController::OnMove);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKazanPlayerController::OnLook);

		// Attack
		EnhancedInputComponent->BindAction(AttackCAction, ETriggerEvent::Started, this,
										   &AKazanPlayerController::On_Attack_Weak_Pressed);
		EnhancedInputComponent->BindAction(AttackSAction, ETriggerEvent::Started, this,
										   &AKazanPlayerController::On_Attack_Strong_Pressed);

		// Parry
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &AKazanPlayerController::On_Parry_Pressed);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Completed, this, &AKazanPlayerController::On_Parry_Released);

		// Evade
		EnhancedInputComponent->BindAction(EvadeAction, ETriggerEvent::Started, this, &AKazanPlayerController::On_Dodge_Pressed);
	}
	else
	{
		UE_LOG(LogTemp, Error,
			   TEXT(
				   "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
			   ), *GetNameSafe(this));
	}
}

void AKazanPlayerController::OnMove(const FInputActionValue& Value)
{
	Kazan->Move(Value.Get<FVector2D>());
}

void AKazanPlayerController::OnLook(const FInputActionValue& Value)
{
	Kazan->Look(Value.Get<FVector2D>());
}

void AKazanPlayerController::On_Attack_Weak_Pressed(const FInputActionValue& InputActionValue)
{
	Kazan->Attack_Weak_Pressed();
}

void AKazanPlayerController::On_Attack_Strong_Pressed(const FInputActionValue& InputActionValue)
{
	Kazan->Attack_Strong_Pressed();
}

void AKazanPlayerController::On_Parry_Pressed(const FInputActionValue& InputActionValue)
{
	Kazan->Parry_Pressed();
}

void AKazanPlayerController::On_Parry_Released(const FInputActionValue& InputActionValue)
{
	Kazan->Parry_Released();
}

void AKazanPlayerController::On_Dodge_Pressed(const FInputActionValue& InputActionValue)
{
	Kazan->Dodge_Pressed();
}

void AKazanPlayerController::On_Run_Pressed(const FInputActionValue& InputActionValue)
{
}

void AKazanPlayerController::On_Run_Released(const FInputActionValue& InputActionValue)
{
}
