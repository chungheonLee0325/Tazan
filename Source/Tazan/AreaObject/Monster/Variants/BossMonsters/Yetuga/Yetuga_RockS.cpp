// Fill out your copyright notice in the Description page of Project Settings.


#include "Yetuga_RockS.h"

#include "Yetuga.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProfilingDebugging/CookStats.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/Utilities/LogMacro.h"


class AYetuga;

AYetuga_RockS::AYetuga_RockS()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetSphereRadius(52.0f);
	
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetWorldScale3D(FVector(0.4f));
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh>RockMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/_Resource/Yetuga/Rock/Yetuga_SmallRock.Yetuga_SmallRock'"));
	if (RockMesh.Succeeded())
	{
		Mesh->SetSkeletalMesh(RockMesh.Object);
	}
	
	ConstructorHelpers::FClassFinder<UAnimInstance> TempABP(TEXT("/Script/Engine.AnimBlueprintGeneratedClass'/Game/_BluePrints/AreaObject/Monster/BossMonsters/ABP_Rock.ABP_Rock_C'"));
	if (TempABP.Succeeded())
	{
		Mesh->SetAnimInstanceClass(TempABP.Class);
	}
	
	ProjectileMovement=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->ProjectileGravityScale = 0.9f;

	Root->SetGenerateOverlapEvents(true);
	
	Root->SetCollisionObjectType(ECC_EngineTraceChannel2);

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
	AYetuga* yetuga = Cast<AYetuga>(OtherActor);
	if (yetuga)
	{
		return;
	}

	if (bIsHitOnce)
	{
		return;
	}
	
	// LOG_SCREEN("돌 오버랩");
	bIsHitOnce = true;
	
	if (OtherActor == Target)
	{
		// Caster->CalcDamage(*AttackData->IndexedAttackData, Caster, hitActor, SweepResult);
		UAnimInstance* animInst = Mesh->GetAnimInstance();

		if (TargetDestroyAni != nullptr)
		{
			animInst->Montage_Play(TargetDestroyAni);
		}
	}
	else
	{
		UAnimInstance* animInst = Mesh->GetAnimInstance();
		
		if (FloorDestroyAni != nullptr)
		{
			animInst->Montage_Play(TargetDestroyAni);
		}
	}
}

void AYetuga_RockS::InitRock(ABaseMonster* caster, AAreaObject* player)
{
	Caster = caster;
	Target = player;
}

void AYetuga_RockS::Fire()
{
	if (!Caster || !Target) return;

	FVector StartLoc = GetActorLocation();
	FVector TargetLoc = Target->GetActorLocation();

	// 발사체 위치를 시작 위치로 이동
	SetActorLocation(StartLoc);

	FVector OutVelocity;
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity(
		this,
		OutVelocity,
		StartLoc,
		TargetLoc,
		3300.0f,
		false,
		0.0f,
		GetWorld()->GetGravityZ(),
		ESuggestProjVelocityTraceOption::DoNotTrace
	);

	if (bHaveAimSolution)
	{
		ProjectileMovement->Velocity = OutVelocity;
		ProjectileMovement->UpdatedComponent = Root;
		ProjectileMovement->Activate();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("YetugaBall, No valid parabola solution!"));
	}
}


void AYetuga_RockS::DoDestroy()
{
	Destroy();
}

