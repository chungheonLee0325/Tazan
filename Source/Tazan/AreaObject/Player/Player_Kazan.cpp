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
#include "Tazan/Utilities/LogMacro.h"


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
		WeaponComponent->ComponentTags.Add(TEXT("WeaponMesh"));
		static ConstructorHelpers::FObjectFinder<UMaterial> tempWeaponMaterial(
			TEXT("/Script/Engine.Material'/Game/_Resource/Weapon/CM_I_GSword_Ruins001.CM_I_GSword_Ruins001'"));
		if (tempWeaponMaterial.Succeeded())
		{
			WeaponComponent->SetMaterial(0, tempWeaponMaterial.Object);
		}
		//FTransform attachTransform = GetMesh()->GetSocketTransform(TEXT("Weapon_R_BackPack_GSword"));
		//WeaponComponent->SetRelativeTransform(attachTransform);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempDodgeAnimMontage(TEXT(
		"/Script/Engine.AnimMontage'/Game/_Resource/Kazan/Animation/GSword/CA_P_Kazan_GSword_Dodge_F_Montage.CA_P_Kazan_GSword_Dodge_F_Montage'"));
	if (tempDodgeAnimMontage.Succeeded())
	{
		DodgeAnimMontage = tempDodgeAnimMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempBackDodgeAnimMontage(TEXT(
		"/Script/Engine.AnimMontage'/Game/_Resource/Kazan/Animation/GSword/CA_P_Kazan_GSword_Dodge_B_Montage.CA_P_Kazan_GSword_Dodge_B_Montage'"));
	if (tempBackDodgeAnimMontage.Succeeded())
	{
		BackDodgeAnimMontage = tempBackDodgeAnimMontage.Object;
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
	CameraBoom->SetRelativeLocation({0, 0, 40});

	CameraBoom->TargetArmLength = 300.0f; // The Camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	// Camera Lagging
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.0f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->FieldOfView = 100;
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

	//GetCharacterMovement()->SetMovementMode(MOVE_None);
}

// Called every frame
void APlayer_Kazan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayer_Kazan::InitializeStateRestrictions()
{
	// 일반 상태 - 모든 행동 가능
	FActionRestrictions NormalRestrictions;
	StateRestrictions.Add(EPlayerState::NORMAL, NormalRestrictions);

	// 스킬 준비 중 - 이동, 회전 제한
	FActionRestrictions SkillPrepareRestrictions;
	SkillPrepareRestrictions.bCanMove = false;
	SkillPrepareRestrictions.bCanRotate = false;
	StateRestrictions.Add(EPlayerState::SKILL_PREPARE, SkillPrepareRestrictions);

	// 스킬 시전 중 - 이동, 회전 제한
	FActionRestrictions SkillCastingRestrictions;
	SkillCastingRestrictions.bCanMove = false;
	SkillCastingRestrictions.bCanRotate = false;
	StateRestrictions.Add(EPlayerState::SKILL_PREPARE, SkillCastingRestrictions);

	// 가드 중 - 달리기, 공격 제한
	FActionRestrictions GuardingRestrictions;
	GuardingRestrictions.bCanAttack = false;
	GuardingRestrictions.bCanUseSkills = false;
	StateRestrictions.Add(EPlayerState::GUARDING, GuardingRestrictions);

	// 회피 중 - 대부분의 행동 제한
	FActionRestrictions EvadingRestrictions;
	EvadingRestrictions.bCanAttack = false;
	EvadingRestrictions.bCanGuard = false;
	EvadingRestrictions.bCanUseSkills = false;
	StateRestrictions.Add(EPlayerState::EVADING, EvadingRestrictions);

	// 스턴 상태 - 모든 행동 제한 (카메라만 가능)
	FActionRestrictions StagerRestrictions;
	StagerRestrictions.bCanMove = false;
	StagerRestrictions.bCanRotate = false;
	StagerRestrictions.bCanAttack = false;
	StagerRestrictions.bCanGuard = false;
	StagerRestrictions.bCanEvade = false;
	StagerRestrictions.bCanUseSkills = false;
	StateRestrictions.Add(EPlayerState::STAGERING, StagerRestrictions);
}

bool APlayer_Kazan::CanPerformAction(EPlayerState State, FString ActionName)
{
	if (!StateRestrictions.Contains(State))
		return false;

	const FActionRestrictions& Restrictions = StateRestrictions[State];

	if (ActionName == "Move")
		return Restrictions.bCanMove;
	else if (ActionName == "Rotate")
		return Restrictions.bCanRotate;
	else if (ActionName == "Look")
		return Restrictions.bCanLook;
	else if (ActionName == "Attack")
		return Restrictions.bCanAttack;
	else if (ActionName == "Guard")
		return Restrictions.bCanGuard;
	else if (ActionName == "Evade")
		return Restrictions.bCanEvade;
	else if (ActionName == "UseSkill")
		return Restrictions.bCanUseSkills;

	return false;
}

void APlayer_Kazan::SetPlayerState(EPlayerState NewState)
{
	CurrentPlayerState = NewState;

	// 상태 변경에 따른 추가 처리
	const FActionRestrictions& NewRestrictions = StateRestrictions[NewState];

	// 이동 제한 적용
	if (!NewRestrictions.bCanMove)
		GetCharacterMovement()->DisableMovement();
	else
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// 회전 제한 적용
	GetCharacterMovement()->bOrientRotationToMovement = NewRestrictions.bCanRotate;
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

		// add Movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		// Rotate Only
		//FRotator NewRotation = FRotationMatrix::MakeFromX(RightDirection * MovementVector.X + ForwardDirection * MovementVector.Y).Rotator();
		//SetActorRotation(NewRotation);
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

void APlayer_Kazan::Attack_Weak_Pressed()
{
	TObjectPtr<UBaseSkill> skill = GetSkillByID(10);
	if (true == CanCastSkill(skill, this))
	{
		CastSkill(skill, this);
	}
}

void APlayer_Kazan::Attack_Strong_Pressed()
{
	CastSkill(GetSkillByID(20), this);
}

void APlayer_Kazan::Guard_Pressed()
{
	// 애니메이션 변수 셋팅
	KazanAnimInstance->bIsGuard = true;

	// ToDo : @@LCH 고민 바로 적용이 맞는지 Notify로 빼서 적용할지
	// 플레이어 셋팅
	// 가드 상태 추가
	SetGuardState(true);
	// 가드 상태이상 추가
	AddCondition(EConditionBitsType::Guard);
	// 퍼펙트 가드 상태이상 추가
	AddCondition(EConditionBitsType::PerfectGuardWindow, 0.1f);

	// 이동속도 셋팅
	GetCharacterMovement()->MaxWalkSpeed = MAX_GUARD_WALK_SPEED;
	// Rotation Setting
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void APlayer_Kazan::Guard_Released()
{
	KazanAnimInstance->bIsGuard = false;

	// 플레이어 셋팅
	SetGuardState(false);
	// 가드 상태이상 제거
	RemoveCondition(EConditionBitsType::Guard);
	// 이동속도 셋팅
	GetCharacterMovement()->MaxWalkSpeed = MAX_WALK_SPEED;
	// Rotation Setting
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void APlayer_Kazan::Dodge_Pressed()
{
	if (CanDodge == false)
	{
		return;
	}
	if (!CanUseStamina(DODGE_COST))
	{
		return;
	}
	CanDodge = false;
	DecreaseStamina(DODGE_COST);

	if (GetCharacterMovement()->Velocity.Length() > 0.1f)
	{
		PlayAnimMontage(DodgeAnimMontage);
	}
	else
	{
		PlayAnimMontage(BackDodgeAnimMontage);
	}

	TWeakObjectPtr<APlayer_Kazan> weakThis = this;
	GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle, [weakThis]()
	{
		APlayer_Kazan* StrongThis = weakThis.Get();
		if (StrongThis != nullptr)
		{
			StrongThis->CanDodge = true;
		}
	}, DodgeCoolTime, false);
}

void APlayer_Kazan::On_Run_Pressed()
{
}

void APlayer_Kazan::On_Run_Released()
{
}
