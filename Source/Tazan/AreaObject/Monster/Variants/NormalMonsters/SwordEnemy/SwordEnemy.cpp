// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordEnemy.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/SwordMonster/Sword_FSM.h"


// Sets default values
ASwordEnemy::ASwordEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_AreaObjectID = 10;

	//FSM 담기 
	m_AiFSM = ASwordEnemy::CreateFSM();

	//스켈레탈 메쉬 값조정 
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh
		(TEXT("'/Game/_Resource/SkeletonSword/C_M_SkeletonSword.C_M_SkeletonSword'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
		GetMesh()->SetRelativeScale3D(FVector(0.45f));
	}
	ConstructorHelpers::FObjectFinder<UMaterial> TempMesh_Mt(
		TEXT("'/Game/_Resource/SkeletonSword/CM_M_Skeleton.CM_M_Skeleton'"));
	if (TempMesh_Mt.Succeeded())
	{
		GetMesh()->SetMaterial(0, TempMesh_Mt.Object);
	}


	//애님 생성
	ConstructorHelpers::FClassFinder<UAnimInstance> TempAnim(TEXT(
		"'/Game/_BluePrints/AreaObject/Monster/SwordMonsters/BluePrints/ABP_SwordMonster.ABP_SwordMonster_C'"));
	if (TempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(TempAnim.Class);
	}


	//소드컴프 생성및 값조정
	SwordComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordComp"));
	SwordComp->SetupAttachment(GetMesh(), (TEXT("Weapon_R")));

	SwordComp->SetRelativeLocation(FVector(7.000000, -2.000000, -10.000000));
	SwordComp->SetRelativeRotation(FRotator(-7.000000, -19.000000, 0.000000));
	SwordComp->SetRelativeScale3D(FVector(1.200000, 1.000000, 0.880000));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSword(TEXT("'/Game/_Resource/Sword/SKM_Sword.SKM_Sword'"));
	if (TempSword.Succeeded())
	{
		SwordComp->SetSkeletalMesh(TempSword.Object);
	}
	//소드머터리얼 설정
	ConstructorHelpers::FObjectFinder<UMaterial> TempSword_Mt(
		TEXT("'/Game/_Resource/SkeletonSword/CM_I_SkeletonSword.CM_I_SkeletonSword'"));
	if (TempSword_Mt.Succeeded())
	{
		SwordComp->SetMaterial(0, TempSword_Mt.Object);
	}
	SwordComp->ComponentTags.Add(TEXT("WeaponMesh"));

	//캡슐컴포넌트 크기조정
	GetCapsuleComponent()->SetCapsuleRadius(50.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(105.f);

	// 무브먼트에 아웃라이너 언리얼 제공 값들 가져오기 
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	float CurrentMaxWalkSpeed = characterMovement->MaxWalkSpeed;

	// 가져온것: 걷기최대속도 , 회전 바라보는방향 자동화 
	characterMovement->MaxWalkSpeed = 350.0f;
	characterMovement->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void ASwordEnemy::BeginPlay()
{
	Super::BeginPlay();
}

UBaseAiFSM* ASwordEnemy::CreateFSM()
{
	return CreateDefaultSubobject<USword_FSM>(TEXT("AiFSM"));
}

// Called every frame
void ASwordEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASwordEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
