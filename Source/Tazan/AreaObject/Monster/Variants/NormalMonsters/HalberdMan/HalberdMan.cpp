// Fill out your copyright notice in the Description page of Project Settings.


#include "HalberdMan.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/HalberdMan/HalberdManFSM.h"


// Sets default values
AHalberdMan::AHalberdMan()
{
	PrimaryActorTick.bCanEverTick = true;

	// AreaObject ID Setting
	m_AreaObjectID = 20;

	// FSM Setting
	m_AiFSM = AHalberdMan::CreateFSM();
	// Skill Setting
	m_SkillRoulette = AHalberdMan::CreateSkillRoulette();

	ParryStackMax = 2;

	// CapsuleComponent Setting
	GetCapsuleComponent()->SetCapsuleHalfHeight(150.f);
	GetCapsuleComponent()->SetCapsuleRadius(50.f);

	// 최대 걷기 속도
	GetCharacterMovement()->MaxWalkSpeed = 450.0f;
	
	// SkeletalMesh Setting
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempSkeletalMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/_Resource/Halberd/Halberd.Halberd'"));
	if (tempSkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempSkeletalMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -147.f), FRotator(0, -90, 0));
		GetMesh()->SetRelativeScale3D(FVector(0.4f));
	}

	// WeaponMesh Setting	
	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponComponent->SetupAttachment(GetMesh(),TEXT("Weapon_R"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempWeaponSkeletalMesh(TEXT(
		"/Script/Engine.SkeletalMesh'/Game/_Resource/Halberd/Weapons/C_I_Halberd_Weapon.C_I_Halberd_Weapon'"));
	if (tempWeaponSkeletalMesh.Succeeded())
	{
		WeaponComponent->SetSkeletalMesh(tempWeaponSkeletalMesh.Object);
		WeaponComponent->SetRelativeScale3D(FVector(0.4f));
		WeaponComponent->ComponentTags.Add(TEXT("WeaponMesh"));
		static ConstructorHelpers::FObjectFinder<UMaterial> tempWeaponMaterial(
			TEXT("/Script/Engine.Material'/Game/_Resource/Halberd/Weapons/CM_I_Halberd_Weapon.CM_I_Halberd_Weapon'"));
		if (tempWeaponMaterial.Succeeded())
		{
			WeaponComponent->SetMaterial(0, tempWeaponMaterial.Object);
		}
	}

	// Set Animation Blueprint
	ConstructorHelpers::FClassFinder<UAnimInstance> TempABP(TEXT(
		"/Script/Engine.AnimBlueprint'/Game/_BluePrints/AreaObject/Monster/HalberdMan/ABP_HalberdMan.ABP_HalberdMan_C'"));

	if (TempABP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(TempABP.Class);
	}
}


// Called when the game starts or when spawned
void AHalberdMan::BeginPlay()
{
	Super::BeginPlay();
	
}

UBaseAiFSM* AHalberdMan::CreateFSM()
{
	return CreateDefaultSubobject<UHalberdManFSM>(TEXT("FSM"));
}

UBaseSkillRoulette* AHalberdMan::CreateSkillRoulette()
{
	return Super::CreateSkillRoulette();
}

void AHalberdMan::HandleGroggy(float Duration)
{
	if (IsDie())
		return;
	IsGroggy = true;
	m_PoiseComponent->SetCurrentStagger(EStaggerType::Groggy);
	HandleStaggerBegin(EStaggerType::Groggy,FName("Default"));
}

void AHalberdMan::HandleStaggerEnd()
{
	if (IsGroggy)
	{
		OnGroggyEnd();
		IncreaseStamina(3000.f);
	}
	Super::HandleStaggerEnd();
}


// Called every frame
void AHalberdMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHalberdMan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

