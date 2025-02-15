// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Kazan.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MaterialHLSLTree.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Layers/LayersSubsystem.h"
#include "Tazan/Animation/Player/KazanAniminstance.h"


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
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempSkeletalMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/_Resource/Kazan/SM_Kazan.SM_Kazan'"));
	if (tempSkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempSkeletalMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -97.f), FRotator(0, -90, 0));
		GetMesh()->SetRelativeScale3D(FVector(0.4f));
	}

	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponComponent->SetupAttachment(GetMesh(),TEXT("Weapon_R"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempWeaponSkeletalMesh(TEXT(
		"/Script/Engine.SkeletalMesh'/Game/_Resource/Weapon/Gsword_C_I_GSword_Ruins001.Gsword_C_I_GSword_Ruins001'"));
	if (tempWeaponSkeletalMesh.Succeeded())
	{
		WeaponComponent->SetSkeletalMesh(tempWeaponSkeletalMesh.Object);
		WeaponComponent->SetRelativeScale3D(FVector(0.4f));
		//FTransform attachTransform = GetMesh()->GetSocketTransform(TEXT("Weapon_R_BackPack_GSword"));
		//WeaponComponent->SetRelativeTransform(attachTransform);
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
	GetCharacterMovement()->MaxWalkSpeed = MAX_WALK_SPEED;
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
	CameraBoom->CameraLagSpeed = 10.0f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void APlayer_Kazan::BeginPlay()
{
	Super::BeginPlay();

	KazanAnimInstance = Cast<UKazanAniminstance>(GetMesh()->GetAnimInstance());
	
}

void APlayer_Kazan::OnDie()
{
	Super::OnDie();
	
	CanMove = false;
	CanRotate = false;
	CanSkill = false;
	GetCharacterMovement()->SetMovementMode(MOVE_None);
}

// Called every frame
void APlayer_Kazan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void APlayer_Kazan::Move(const FVector2D MovementVector)
{
	// input is a Vector2D
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

void APlayer_Kazan::Look(const FVector2D LookAxisVector)
{
	// input is a Vector2D
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		// 좌우 회전
		AddControllerYawInput(LookAxisVector.X * LookSensitivityX);
		// 상하 회전 제한 적용
		float newPitchAngle = CurrentPitchAngle + (LookAxisVector.Y * LookSensitivityY);
		newPitchAngle = FMath::ClampAngle(newPitchAngle, MinPitchAngle, MaxPitchAngle);

		float pitchInput = newPitchAngle - CurrentPitchAngle;
		
		AddControllerPitchInput(pitchInput);

		CurrentPitchAngle = newPitchAngle;
	}
}

void APlayer_Kazan::On_Attack_Common_Pressed()
{
}

void APlayer_Kazan::On_Attack_Strong_Pressed()
{
}

void APlayer_Kazan::Parry_Pressed()
{
	// 애니메이션 변수 셋팅
	KazanAnimInstance->bIsGuard = true;
	
	// ToDo : @@LCH 고민 바로 적용이 맞는지 Notify로 빼서 적용할지
	// 플레이어 셋팅
	IsGuard = true;
	// 이동속도 셋팅
	GetCharacterMovement()->MaxWalkSpeed = MAX_GUARD_WALK_SPEED;
	// Rotation Setting
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void APlayer_Kazan::Parry_Released()
{
	KazanAnimInstance->bIsGuard = false;
	
	// 플레이어 셋팅
	IsGuard = false;
	// 이동속도 셋팅
	GetCharacterMovement()->MaxWalkSpeed = MAX_WALK_SPEED;
	// Rotation Setting
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void APlayer_Kazan::On_Evade_Pressed()
{
}

void APlayer_Kazan::On_Run_Pressed()
{
}

void APlayer_Kazan::On_Run_Released()
{
}

void APlayer_Kazan::PerfectParryActivated()
{
	
}

void APlayer_Kazan::PerfectParryDeactivated()
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

void APlayer_Kazan::DodgeInvincibilityStart()
{
}

void APlayer_Kazan::DodgeInvincibilityEnd()
{
}
