// Fill out your copyright notice in the Description page of Project Settings.


#include "Burrbo.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/Burrbo/AiBurrbo.h"
#include "Tazan/AreaObject/Skill/Monster/JumpAttack.h"


// Sets default values
ABurrbo::ABurrbo()
{
	m_AreaObjectID = 100;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCapsuleHalfHeight(50.f);
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/_Resource/Monster/Burrbo/Burrbo.Burrbo'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	GetMesh()->SetRelativeScale3D(FVector(0.3f));
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f,0.0f,-50.0f),FRotator(0.0f,-90.0f,0.0f));
	
	BodyCollider = CreateDefaultSubobject<USphereComponent>(FName("BodyCollider"));
	BodyCollider->SetupAttachment(GetMesh());
	BodyCollider->SetRelativeScale3D(FVector(5.3f));
	BodyCollider->SetRelativeLocation(FVector(0.0f,0.0f,160.0f));
	BodyCollider->SetCollisionProfileName(TEXT("Enemy"));

	m_StateSkillClasses.Add(EAiStateType::Attack, UJumpAttack::StaticClass());
	m_AiFSM = ABurrbo::CreateFSM();
}

// Called when the game starts or when spawned
void ABurrbo::BeginPlay()
{
	Super::BeginPlay();

	BodyCollider->OnComponentBeginOverlap.AddDynamic(this, &ABurrbo::OnBodyBeginOverlap);
}

UBaseAiFSM* ABurrbo::CreateFSM()
{
	return CreateDefaultSubobject<UAiBurrbo>(TEXT("AiFSM"));
}

// Called every frame
void ABurrbo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABurrbo::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ABurrbo::GetDetectRange() const
{
	return DetectRange;
}



