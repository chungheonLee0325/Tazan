// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/SwordMonster/Sword_FSM.h"


// Sets default values
ASwordEnemy::ASwordEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_AreaObjectID = 10;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/_Resource/SkeletonSword/C_M_SkeletonSword.C_M_SkeletonSword'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
		GetMesh()->SetRelativeScale3D(FVector(0.37f));
	}
	m_AiFSM = ASwordEnemy::CreateFSM();
	// 무브먼트에 아웃라이너 언리얼 제공 값들 가져오기 
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	float CurrentMaxWalkSpeed = characterMovement->MaxWalkSpeed;
	// 가져온것: 걷기최대속도 , 회전 바라보는방향 자동화 
	characterMovement->MaxWalkSpeed = 350.0f;
	characterMovement->bOrientRotationToMovement = true;

	ConstructorHelpers::FClassFinder<UAnimInstance> TempAnim(TEXT("'/Game/_BluePrints/AreaObject/Monster/SwordMonsters/BluePrints/ABP_SwordMonster.ABP_SwordMonster'"));
	if (TempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(TempAnim.Class);
	}
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
