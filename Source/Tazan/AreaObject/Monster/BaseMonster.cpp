// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMonster.h"

#include "AI/Base/BaseAiFSM.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/SkillRoulette.h"
#include "Tazan/Contents/TazanGameInstance.h"


// Sets default values
ABaseMonster::ABaseMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SkillRoulette = CreateDefaultSubobject<USkillRoulette>(TEXT("SkillRoulette"));
	
	// Death Effect Load
	//static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(
	//	TEXT("/Script/Engine.ParticleSystem'/Game/_Resource/FX/Realistic_Starter_VFX_Pack_Vol2/Particles/Destruction/P_Destruction_Electric.P_Destruction_Electric'"));
	//if (ParticleAsset.Succeeded())
	//{
	//	DeathEffect = ParticleAsset.Object;
	//}
}

// Called when the game starts or when spawned
void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();

	UTazanGameInstance* gameInstance = Cast<UTazanGameInstance>(GetGameInstance());
	int skillbagID = dt_AreaObject->SkillBagID;
	if (skillbagID != 0)
	{
		dt_SkillBag = gameInstance->GetDataSkillBag(skillbagID);
		if (SkillRoulette != nullptr)
		{
			SkillRoulette->InitFromSkillBag(dt_SkillBag);
		}
		else
		{
			LOG_PRINT(TEXT("스킬룰렛 없음"));
		}
	}
	else
	{
		LOG_PRINT(TEXT("SkillBag is 0"));
	}

	m_SpawnLocation = GetActorLocation();
	
	if (m_AiFSM != nullptr)
	{
		m_AiFSM->InitStatePool();
	}
}

// Called every frame
void ABaseMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseMonster::OnBodyBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult)
{
}

UBaseAiFSM* ABaseMonster::CreateFSM()
{
	return nullptr;
}

bool ABaseMonster::IsMoving() const
{
	return true;
}

bool ABaseMonster::IsRotating() const
{
	return true;
}

AAreaObject* ABaseMonster::GetAggroTarget() const
{
	return m_AggroTarget;
}

float ABaseMonster::GetDistToTarget()
{
	if (m_AggroTarget == nullptr)
	{
		LOG_PRINT(TEXT("AggroTarget is null."));
		return 0;
	}
	return GetDistanceTo(m_AggroTarget);
}

FVector ABaseMonster::GetDirToTarget()
{
	if (m_AggroTarget == nullptr)
	{
		LOG_PRINT(TEXT("AggroTarget is null."));
		return FVector::ZeroVector;
	}
	return m_AggroTarget->GetActorLocation()-GetActorLocation();
}

void ABaseMonster::OnDie()
{
	Super::OnDie();

	// FSM 정지
	if (m_AiFSM != nullptr)	m_AiFSM->StopFSM();
	// Skill 정지
	if (nullptr != m_CurrentSkill) m_CurrentSkill->CancelCast();
	// 움직임 정지
	StopAll();
}

void ABaseMonster::AddParryStack()
{
	++ParryStack;
	// LOG_SCREEN("패리 스택: %d",ParryStack);
	if (ParryStack == ParryStackMax)
	{
		ParryStackPenalty();
	}
}

void ABaseMonster::ParryStackPenalty()
{
	// LOG_SCREEN("패리 패널티!");
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst)
	{
		if (ParryPenaltyAnimation != nullptr)
		{
			animInst->Montage_Play(ParryPenaltyAnimation);
		}
		else
		{
			LOG_SCREEN_ERROR(this, "패리 패널티 애니 비어있음");
		}
	}
	ParryStack = 0;
}

void ABaseMonster::InitParryStack()
{
	ParryStack = 0;
}

void ABaseMonster::RemoveSkillEntryByID(const int id)
{
	SkillRoulette->RemoveSkillEntryByID(id);
}

void ABaseMonster::AddSkillEntryByID(const int id)
{
	SkillRoulette->AddSkillEntryByID(id);
}