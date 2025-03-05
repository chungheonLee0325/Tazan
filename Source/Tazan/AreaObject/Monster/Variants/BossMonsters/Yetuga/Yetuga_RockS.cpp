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
	Mesh->SetWorldScale3D(FVector(0.25f));
	
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
	
	if (OtherActor == Target)
	{
		// Caster->CalcDamage(*AttackData->IndexedAttackData, Caster, hitActor, SweepResult);
		UAnimInstance* animInst = Mesh->GetAnimInstance();

		if (!animInst)
		{
			LOG_SCREEN_ERROR(this, "AnimInstance is null");
			return;
		}

		if (TargetDestroyAni != nullptr)
		{
			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &AYetuga_RockS::OnDestroy);
		
			animInst->Montage_SetEndDelegate(MontageEndedDelegate, TargetDestroyAni);
			animInst->Montage_Play(TargetDestroyAni);
		}
		else
		{
			LOG_SCREEN_ERROR(this, "애니 몽타주 널");
		}
		return;
	}

	if (FloorDestroyAni != nullptr)
	{
		UAnimInstance* animInst = Mesh->GetAnimInstance();
	
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &AYetuga_RockS::OnDestroy);
		
		animInst->Montage_SetEndDelegate(MontageEndedDelegate, FloorDestroyAni);
		Mesh->GetAnimInstance()->Montage_Play(FloorDestroyAni);
	}
	else
	{
		LOG_SCREEN_ERROR(this, "애니 몽타주 널");
	}
}

void AYetuga_RockS::InitRock(ABaseMonster* caster, AAreaObject* player)
{
	Caster = caster;
	Target = player;
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
		FPredictProjectilePathParams predictParams(20.0f, startLoc, outVelocity*100.0f, 15.0f);
		
		predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
		FPredictProjectilePathResult result;
		UGameplayStatics::PredictProjectilePath(this, predictParams, result);
		
		ProjectileMovement->Velocity = outVelocity;
		ProjectileMovement->Activate();
	}
}

void AYetuga_RockS::OnDestroy(UAnimMontage* AnimMontage, bool bArg)
{
	LOG_SCREEN("돌 파괴");
	Destroy();
}

