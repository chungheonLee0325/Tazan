﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Kazan.h"
#include "KazanPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Tazan/Animation/Player/KazanAnimInstance.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/AreaObject/Utility/GhostTrail.h"
#include "Tazan/Utilities/LogMacro.h"


class UEnhancedInputLocalPlayerSubsystem;
// Sets default values
APlayer_Kazan::APlayer_Kazan()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set AreaObject ID
	m_AreaObjectID = 1;

	// Set SFX ID
	GuardSFXID = 10;
	PerfectGuardSFXID = 11;

	// DamagedType 설정 - FloatingText 설정
	m_DefaultDamageType = EFloatingDamageType::PlayerDamaged;

	// Die Setting
	DestroyDelayTime = 3.0f;

	// Set Size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(30.f, 96.f);

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
	}

	// Set Animation Blueprint
	ConstructorHelpers::FClassFinder<UAnimInstance> TempABP(TEXT(
		"/Script/Engine.AnimBlueprint'/Game/_BluePrints/AreaObject/Player/ABP_Player_Kazan_AnimInstance.ABP_Player_Kazan_AnimInstance_C'"));

	if (TempABP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(TempABP.Class);
	}

	// Set Anim Montage
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> tempDodgeAnimMontage(TEXT(
	//	"/Script/Engine.AnimMontage'/Game/_Resource/Kazan/Animation/GSword/CA_P_Kazan_GSword_Dodge_F_Montage.CA_P_Kazan_GSword_Dodge_F_Montage'"));
	//if (tempDodgeAnimMontage.Succeeded())
	//{
	//	DodgeAnimMontage = tempDodgeAnimMontage.Object;
	//}
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> tempBackDodgeAnimMontage(TEXT(
	//	"/Script/Engine.AnimMontage'/Game/_Resource/Kazan/Animation/GSword/CA_P_Kazan_GSword_Dodge_B_Montage.CA_P_Kazan_GSword_Dodge_B_Montage'"));
	//if (tempBackDodgeAnimMontage.Succeeded())
	//{
	//	BackDodgeAnimMontage = tempBackDodgeAnimMontage.Object;
	//}

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

void APlayer_Kazan::SpecialFUNCTION()
{
	if (!IsSpecial)
	{
		IsSpecial = true;
		TWeakObjectPtr<APlayer_Kazan> weakThis = this;
		FTransform SpawnTransform = GetMesh()->GetComponentTransform();

		AGhostTrail* GhostTrail = GetWorld()->SpawnActor<AGhostTrail>(AGhostTrail::StaticClass(), SpawnTransform);

		if (GhostTrail)
		{
			GhostTrail->InitByMaterials(GetMesh(), 0.5, 0.2);
		}

		GetWorld()->GetTimerManager().SetTimer(SpecialTimerHandle, [weakThis]()
		{
			if (auto* StrongThis = weakThis.Get())
			{
				FTransform SpawnTransform = StrongThis->GetMesh()->GetComponentTransform();

				AGhostTrail* GhostTrail = StrongThis->GetWorld()->SpawnActor<AGhostTrail>(
					AGhostTrail::StaticClass(),
					SpawnTransform
				);

				if (GhostTrail)
				{
					GhostTrail->InitByMaterials(StrongThis->GetMesh(), 0.5, 0.2);
				}
			}
		}, 0.2f, true);
		m_HealthComponent->InitHealth(10000000);
	}
	else
	{
		IsSpecial = false;
		GetWorld()->GetTimerManager().ClearTimer(SpecialTimerHandle);
		m_HealthComponent->InitHealth(dt_AreaObject->HPMax);
	}
}

// Called when the game starts or when spawned
void APlayer_Kazan::BeginPlay()
{
	Super::BeginPlay();

	KazanAnimInstance = Cast<UKazanAnimInstance>(GetMesh()->GetAnimInstance());
	KazanPlayerController = Cast<AKazanPlayerController>(GetController());

	InitializeStateRestrictions();
}

void APlayer_Kazan::OnDie()
{
	Super::OnDie();

	//GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void APlayer_Kazan::HandleStaggerEnd()
{
	Super::HandleStaggerEnd();
	SetPlayerState(EPlayerState::NORMAL);
}

void APlayer_Kazan::HandlePerfectDodge()
{
	Super::HandlePerfectDodge();

	TWeakObjectPtr<APlayer_Kazan> weakThis = this;
	FTransform SpawnTransform = GetMesh()->GetComponentTransform();

	AGhostTrail* GhostTrail = GetWorld()->SpawnActor<AGhostTrail>(AGhostTrail::StaticClass(), SpawnTransform);

	if (GhostTrail)
	{
		GhostTrail->InitByMaterials(GetMesh(), 0.5, 0.2);
	}

	FTimerHandle Handle1;
	GetWorld()->GetTimerManager().SetTimer(Handle1, [weakThis]()
	{
		if (auto* StrongThis = weakThis.Get())
		{
			FTransform SpawnTransform = StrongThis->GetMesh()->GetComponentTransform();

			AGhostTrail* GhostTrail = StrongThis->GetWorld()->SpawnActor<AGhostTrail>(
				AGhostTrail::StaticClass(),
				SpawnTransform
			);

			if (GhostTrail)
			{
				GhostTrail->InitByMaterials(StrongThis->GetMesh(), 0.5, 0.2);
			}
		}
	}, 0.2f, false);
}

void APlayer_Kazan::Reward(FItemData* ItemData, int ItemValue) const
{
	if (ItemData->ItemType == EItemType::Currency)
	{
		KazanPlayerController->AddCurrency(ItemData->CurrencyType, ItemValue);
	}
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

	// Only Rotate 상태 - 회전만 가능
	FActionRestrictions OnlyRotateRestrictions;
	OnlyRotateRestrictions.bCanMove = false;
	OnlyRotateRestrictions.bCanOnlyRotate = true;
	OnlyRotateRestrictions.bCanAction = false;
	StateRestrictions.Add(EPlayerState::ONLY_ROTATE, OnlyRotateRestrictions);

	// Action 상태 - 이동, Action 제한
	FActionRestrictions ActionRestrictions;
	ActionRestrictions.bCanMove = false;
	ActionRestrictions.bCanAction = false;
	StateRestrictions.Add(EPlayerState::ACTION, ActionRestrictions);

	// Can Action 상태 - 이동, 제한
	FActionRestrictions CanActionRestrictions;
	CanActionRestrictions.bCanMove = false;
	StateRestrictions.Add(EPlayerState::CANACTION, CanActionRestrictions);

	// Guard 상태 - 회전 제한
	FActionRestrictions GuardRestrictions;
	GuardRestrictions.bCanRotate = false;
	StateRestrictions.Add(EPlayerState::GUARD, GuardRestrictions);

	// Stagger - 삭제할수도?
	FActionRestrictions StaggerRestrictions;
	StaggerRestrictions.bCanMove = false;
	StaggerRestrictions.bCanAction = false;
	StateRestrictions.Add(EPlayerState::STAGER, StaggerRestrictions);

	// Die - 삭제할수도?
	FActionRestrictions DieRestrictions;
	DieRestrictions.bCanMove = false;
	DieRestrictions.bCanAction = false;
	StateRestrictions.Add(EPlayerState::DIE, DieRestrictions);

	SetPlayerState(EPlayerState::NORMAL);
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
	else if (ActionName == "OnlyRotate")
		return Restrictions.bCanOnlyRotate;
	else if (ActionName == "Look")
		return Restrictions.bCanLook;
	else if (ActionName == "Action")
		return Restrictions.bCanAction;

	return false;
}

void APlayer_Kazan::SetComboState(bool bCanCombo, int SkillID)
{
	CanCombo = bCanCombo;
	NextComboSkillID = SkillID;	
}

void APlayer_Kazan::SetPlayerState(EPlayerState NewState)
{
	CurrentPlayerState = NewState;

	// 상태 변경에 따른 추가 처리
	const FActionRestrictions& NewRestrictions = StateRestrictions[NewState];

	// 이동 제한 적용 - Root Motion도 제한하므로 생각해야할듯..
	//if (!NewRestrictions.bCanMove)
	//	GetCharacterMovement()->DisableMovement();
	//else
	//	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// 회전 제한 적용
	GetCharacterMovement()->bOrientRotationToMovement = NewRestrictions.bCanRotate;

	if (NewState == EPlayerState::GUARD)
	{
		SetGuardState(true);
	}
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
		if (CanPerformAction(CurrentPlayerState, "Move"))
		{
			KazanAnimInstance->Montage_Stop(0.2f);
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}

		// Rotate Only
		if (CanPerformAction(CurrentPlayerState, "OnlyRotate"))
		{
			//FRotator NewRotation = FRotationMatrix::MakeFromX(
			//	RightDirection * MovementVector.X + ForwardDirection * MovementVector.Y).Rotator();
			//SetActorRotation(NewRotation);
			FVector targetLocation = GetActorLocation() + RightDirection * MovementVector.X + ForwardDirection *
				MovementVector.Y;
			LookAtLocation(targetLocation, EPMRotationMode::Speed, 1200.f);
		}
	}
}

void APlayer_Kazan::Look(const FVector2D LookAxisVector)
{
	// input is a Vector2D
	if (Controller != nullptr && CanPerformAction(CurrentPlayerState, "Look"))
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
	//LOG_PRINT(TEXT("Input WAttack At %f"), GetWorld()->GetTimeSeconds());
	if (!CanPerformAction(CurrentPlayerState, "Action")) return;

	if (CanCombo && NextComboSkillID)
	{
		TObjectPtr<UBaseSkill> comboSkill = GetSkillByID(NextComboSkillID);
		if (CastSkill(comboSkill,this))
		{
			SetPlayerState(EPlayerState::ACTION);
			comboSkill->OnSkillComplete.BindUObject(this, &APlayer_Kazan::SetPlayerNormalState);
		}
	}

	int weakAttackID = 10;
	TObjectPtr<UBaseSkill> skill = GetSkillByID(weakAttackID);
	if (CastSkill(skill, this))
	{
		SetPlayerState(EPlayerState::ACTION);
		skill->OnSkillComplete.BindUObject(this, &APlayer_Kazan::SetPlayerNormalState);
		//skill->OnSkillCancel.BindUObject(this, &APlayer_Kazan::SetPlayerNormalState);
	}
}

void APlayer_Kazan::Attack_Strong_Pressed()
{
	if (!CanPerformAction(CurrentPlayerState, "Action")) return;
	int strongAttackID = 20;
	TObjectPtr<UBaseSkill> skill = GetSkillByID(strongAttackID);
	if (CastSkill(skill, this))
	{
		SetPlayerState(EPlayerState::ACTION);
		skill->OnSkillComplete.BindUObject(this, &APlayer_Kazan::SetPlayerNormalState);
		//skill->OnSkillCancel.BindUObject(this, &APlayer_Kazan::SetPlayerNormalState);
	}
}

void APlayer_Kazan::Guard_Pressed()
{
	if (!CanPerformAction(CurrentPlayerState, "Action")) return;
	int guardSkillID = 1;
	TObjectPtr<UBaseSkill> skill = GetSkillByID(guardSkillID);
	if (CastSkill(skill, this))
	{
		KazanAnimInstance->bIsGuard = true;
		SetPlayerState(EPlayerState::ACTION);
		//skill->OnSkillComplete.BindUObject(this, &APlayer_Kazan::SetPlayerGuardState);
		//skill->OnSkillCancel.BindUObject(this, &APlayer_Kazan::SetPlayerGuardState);
		bIsGuardRequested = false;
		GetWorld()->GetTimerManager().SetTimer(
			GuardMinDurationTimer,
			this,
			&APlayer_Kazan::TryEndGuard,
			MinGuardDuration,
			false);
	}
	
	//SetGuardState(true);
}

void APlayer_Kazan::Guard_Released()
{
	bIsGuardRequested = true;

	// 최소 지속 시간이 지났는지 확인
	if (!GetWorld()->GetTimerManager().IsTimerActive(GuardMinDurationTimer))
	{
		// 플레이어 셋팅
		SetGuardState(false);
		SetPlayerState(EPlayerState::NORMAL);
		KazanAnimInstance->bIsGuard = false;
	}


}

void APlayer_Kazan::TryEndGuard()
{
	// 가드 해제가 요청된 상태라면 가드 해제
	if (bIsGuardRequested)
	{
		// 플레이어 셋팅
		SetGuardState(false);
		SetPlayerState(EPlayerState::NORMAL);
		KazanAnimInstance->bIsGuard = false;
	}
}

void APlayer_Kazan::Dodge_Pressed()
{
	if (!CanPerformAction(CurrentPlayerState, "Action")) return;
	int dodgeSkillID;
	if (GetCharacterMovement()->Velocity.Length() > 0.1f)
	{
		dodgeSkillID = 3;
	}
	else
	{
		dodgeSkillID = 2;
	}
	TObjectPtr<UBaseSkill> skill = GetSkillByID(dodgeSkillID);
	if (CastSkill(skill, this))
	{
		SetPlayerState(EPlayerState::ACTION);
		skill->OnSkillComplete.BindUObject(this, &APlayer_Kazan::SetPlayerNormalState);
		//skill->OnSkillCancel.BindUObject(this, &APlayer_Kazan::SetPlayerNormalState);
	}

	// Skill 기반 이전 회피 로직
	/*
	//if (CanDodge == false)
	//{
	//	return;
	//}
	//if (!CanUseStamina(DODGE_COST))
	//{
	//	return;
	//}
	//CanDodge = false;
	//DecreaseStamina(DODGE_COST);
	//
	//if (GetCharacterMovement()->Velocity.Length() > 0.1f)
	//{
	//	PlayAnimMontage(DodgeAnimMontage);
	//}
	//else
	//{
	//	PlayAnimMontage(BackDodgeAnimMontage);
	//}
	//
	//TWeakObjectPtr<APlayer_Kazan> weakThis = this;
	//GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle, [weakThis]()
	//{
	//	APlayer_Kazan* StrongThis = weakThis.Get();
	//	if (StrongThis != nullptr)
	//	{
	//		StrongThis->CanDodge = true;
	//	}
	//}, DodgeCoolTime, false);
	*/
}

void APlayer_Kazan::On_Run_Pressed()
{
}

void APlayer_Kazan::On_Run_Released()
{
}

void APlayer_Kazan::SetGuardState(bool bIsGuarding)
{
	// 스태미나 회복률 셋팅
	Super::SetGuardState(bIsGuarding);

	if (bIsGuarding)
	{
		// 애니메이션 변수 셋팅
		KazanAnimInstance->bIsGuard = true;

		// 플레이어 셋팅
		// 가드 Condition 추가
		AddCondition(EConditionBitsType::Guard);
		// 퍼펙트 가드 Condition 추가
		AddCondition(EConditionBitsType::PerfectGuardWindow, 0.1f);

		// Movement Setting
		GetCharacterMovement()->MaxWalkSpeed = MAX_GUARD_WALK_SPEED;
		// Rotation Setting
		//GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		// 가드 Condition 제거
		RemoveCondition(EConditionBitsType::Guard);

		// Movement Setting
		GetCharacterMovement()->MaxWalkSpeed = MAX_WALK_SPEED;
		// Rotation Setting
		//GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}
