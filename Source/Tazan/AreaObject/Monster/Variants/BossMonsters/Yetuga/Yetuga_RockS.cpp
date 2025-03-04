// Fill out your copyright notice in the Description page of Project Settings.


#include "Yetuga_RockS.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/Utilities/LogMacro.h"


AYetuga_RockS::AYetuga_RockS()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetSphereRadius(52.0f);
	
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetWorldScale3D(FVector(0.25f));
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh>RockMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/_Resource/Yetuga/Rock/Yetuga_SmallRock.Yetuga_SmallRock'"));
	if (RockMesh.Succeeded())
	{
		Mesh->SetSkeletalMesh(RockMesh.Object);
	}
	
	ProjectileMovement=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	// ProjectileMovement->bAutoActivate = false;

	Root->SetGenerateOverlapEvents(true);
	
	Root->SetCollisionObjectType(ECC_EngineTraceChannel1);
	Root->SetCollisionResponseToAllChannels(ECR_Ignore);
	Root->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	Root->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Overlap);

	ConstructorHelpers::FObjectFinder<UAnimMontage>am1(TEXT("/Script/Engine.AnimMontage'/Game/_Resource/Yetuga/Rock/AM_RockDestroy_01.AM_RockDestroy_01'"));
	if (am1.Succeeded())
	{
		FloorDestroyAni = am1.Object;
	}

	ConstructorHelpers::FObjectFinder<UAnimMontage>am2(TEXT("/Script/Engine.AnimMontage'/Game/_Resource/Yetuga/Rock/AM_RockDestroy_02.AM_RockDestroy_02'"));
	if (am1.Succeeded())
	{
		TargetDestroyAni = am2.Object;
	}
}

void AYetuga_RockS::BeginPlay()
{
	Super::BeginPlay();
	Root->OnComponentBeginOverlap.AddDynamic(this, &AYetuga_RockS::Overlap);
}

void AYetuga_RockS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AYetuga_RockS::Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	LOG_SCREEN("돌 파괴");
	this->Destroy();
}

void AYetuga_RockS::Fire()
{
	FVector targetLoc = Target->GetActorLocation();
	FVector startLoc = Caster->GetActorLocation();
	float arcValue = 0.75f;

	FVector outVelocity = FVector::ZeroVector;
	if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, startLoc, targetLoc, GetWorld()->GetGravityZ(), arcValue))
	{
		Root->SetGenerateOverlapEvents(true);
		FPredictProjectilePathParams predictParams(20.0f, startLoc, outVelocity, 15.0f);
		
		predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
		FPredictProjectilePathResult result;
		UGameplayStatics::PredictProjectilePath(this, predictParams, result);
		
		ProjectileMovement->Velocity = outVelocity;
		ProjectileMovement->Activate();
	}
}


void AYetuga_RockS::SetCaster(ABaseMonster* caster)
{
	Caster = caster;
}

void AYetuga_RockS::SetTarget(AAreaObject* player)
{
	Target = player;
}

