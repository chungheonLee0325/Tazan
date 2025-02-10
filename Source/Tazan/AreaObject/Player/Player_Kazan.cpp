// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Kazan.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


class UEnhancedInputLocalPlayerSubsystem;
// Sets default values
APlayer_Kazan::APlayer_Kazan()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set AreaObject ID
	m_AreaObjectID = 1;

	// Set Size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// Set Mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempSkeletalMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/_Resource/Kazan/SM_Kazan.SM_Kazan'"));
	if (tempSkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempSkeletalMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -97.f), FRotator(0, -90, 0));
		GetMesh()->SetRelativeScale3D(FVector(0.4f));
	}

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	// Rotation Setting
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // at this rotation rate

	// Movement Setting
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	// Create Camera Boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The Camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	// Camera Lagging
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 3.0f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Enhanced Input Setting
	ConstructorHelpers::FObjectFinder<UInputMappingContext> tempInputMapping(
		TEXT("/Script/EnhancedInput.InputMappingContext'/Game/_Input/IMC_Kazan.IMC_Kazan'"));
	if (tempInputMapping.Succeeded())
	{
		DefaultMappingContext = tempInputMapping.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> tempMoveAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanMove.IA_KazanMove'"));
	if (tempMoveAction.Succeeded())
	{
		MoveAction = tempMoveAction.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> tempLookAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanLook.IA_KazanLook'"));
	if (tempLookAction.Succeeded())
	{
		LookAction = tempLookAction.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> tempAttackCAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanAttack_C.IA_KazanAttack_C'"));
	if (tempAttackCAction.Succeeded())
	{
		AttackCAction = tempAttackCAction.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> tempAttackSAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanAttack_S.IA_KazanAttack_S'"));
	if (tempAttackSAction.Succeeded())
	{
		AttackSAction = tempAttackSAction.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> tempParryAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanParry.IA_KazanParry'"));
	if (tempParryAction.Succeeded())
	{
		ParryAction = tempParryAction.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> tempEvadeAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanEvade.IA_KazanEvade'"));
	if (tempEvadeAction.Succeeded())
	{
		EvadeAction = tempEvadeAction.Object;
	}
}

// Called when the game starts or when spawned
void APlayer_Kazan::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayer_Kazan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayer_Kazan::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called to bind functionality to input
void APlayer_Kazan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayer_Kazan::OnMove);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayer_Kazan::OnLook);

		// Attack
		EnhancedInputComponent->BindAction(AttackCAction, ETriggerEvent::Started, this,
		                                   &APlayer_Kazan::On_Attack_Common_Pressed);
		EnhancedInputComponent->BindAction(AttackSAction, ETriggerEvent::Started, this,
		                                   &APlayer_Kazan::On_Attack_Strong_Pressed);

		// Parry
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &APlayer_Kazan::On_Parry_Pressed);

		// Evade
		EnhancedInputComponent->BindAction(EvadeAction, ETriggerEvent::Started, this, &APlayer_Kazan::On_Evade_Pressed);
	}
	else
	{
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void APlayer_Kazan::OnMove(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayer_Kazan::OnLook(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayer_Kazan::On_Attack_Common_Pressed(const FInputActionValue& InputActionValue)
{
}

void APlayer_Kazan::On_Attack_Strong_Pressed(const FInputActionValue& InputActionValue)
{
}

void APlayer_Kazan::On_Parry_Pressed(const FInputActionValue& InputActionValue)
{
}

void APlayer_Kazan::On_Evade_Pressed(const FInputActionValue& InputActionValue)
{
}

void APlayer_Kazan::OnParryActivated()
{
}

void APlayer_Kazan::OnParryDeactivated()
{
}

void APlayer_Kazan::OnPerfectParryActivated()
{
}

void APlayer_Kazan::OnPerfectParryDeactivated()
{
}

void APlayer_Kazan::OnAttackHitStart()
{
}

void APlayer_Kazan::OnAttackHitEnd()
{
}

void APlayer_Kazan::OnDodgeInvincibilityStart()
{
}

void APlayer_Kazan::OnDodgeInvincibilityEnd()
{
}
