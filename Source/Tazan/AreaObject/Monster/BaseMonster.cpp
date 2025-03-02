﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMonster.h"

#include "AI/Base/BaseAiFSM.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Y_SkillRoulette.h"
#include "Tazan/Contents/TazanGameInstance.h"


// Sets default values
ABaseMonster::ABaseMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	// AI Perception 컴포넌트 생성
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	// 시야 설정 생성 및 구성
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SetMaxAge(0.5f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 0.0f;

	// 팀 설정 - 여기서는 모든 팀을 감지
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// AI Perception 컴포넌트에 시야 설정 추가
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	// 감지 이벤트에 함수 바인딩
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseMonster::OnPerceptionUpdated);
}

UBaseSkillRoulette* ABaseMonster::GetSkillRoulette() const
{
	return m_SkillRoulette;
}

void ABaseMonster::HandleStaggerBegin(EStaggerType Type)
{
	if (m_AiFSM != nullptr) m_AiFSM->ChangeState(EAiStateType::DoNothing);
	Super::HandleStaggerBegin(Type);
}

void ABaseMonster::HandleStaggerEnd()
{
	Super::HandleStaggerEnd();
	if (m_AiFSM != nullptr) m_AiFSM->ChangeState(EAiStateType::SelectSkill);
}

UBaseSkillRoulette* ABaseMonster::CreateSkillRoulette()
{
	return CreateDefaultSubobject<UBaseSkillRoulette>(TEXT("SkillRouletteComponent"));
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
		if (m_SkillRoulette != nullptr)
		{
			m_SkillRoulette->InitFromSkillBag(dt_SkillBag);
		}
		else
		{
			LOG_PRINT(TEXT("스킬룰렛 없음"));
			LOG_SCREEN("SkillRoulette is nullptr. please set in construct");
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
		m_AiFSM->CheckIsValidAiStates();
	}
	else
	{
		LOG_PRINT(TEXT("FSM is nullptr"));
		LOG_SCREEN("FSM is nullptr. please set in construct");
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
	return m_AggroTarget->GetActorLocation() - GetActorLocation();
}

float ABaseMonster::GetNextSkillRange()
{
	return NextSkill == nullptr ? 0.f : NextSkill->GetSkillRange();
}

FVector ABaseMonster::GetSpawnLocation()
{
	return m_SpawnLocation;
}

float ABaseMonster::GetSightLength()
{
	return SightRadius;
}

void ABaseMonster::OnDie()
{
	Super::OnDie();

	// FSM 정지
	if (m_AiFSM != nullptr) m_AiFSM->StopFSM();
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
	HandleStaggerBegin(EStaggerType::ParryReaction);
	/*
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
	*/
}

void ABaseMonster::InitParryStack()
{
	ParryStack = 0;
}

void ABaseMonster::RemoveSkillEntryByID(const int id)
{
	m_SkillRoulette->RemoveSkillEntryByID(id);
}

void ABaseMonster::AddSkillEntryByID(const int id)
{
	m_SkillRoulette->AddSkillEntryByID(id);
}

void ABaseMonster::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 감지된 액터가 유효한지 확인
	if (Actor && Stimulus.WasSuccessfullySensed())
	{
		// 플레이어인지 확인 (플레이어 클래스로 캐스팅)
		APlayer_Kazan* PlayerCharacter = Cast<APlayer_Kazan>(Actor);
		if (PlayerCharacter)
		{
			// 플레이어가 감지되었으므로 현재 타겟으로 설정
			m_AggroTarget = PlayerCharacter;
		}
	}
	// 감지는 Perception으로, Aggro 해제는 fsm에서
	/*
	else if (Actor && !Stimulus.WasSuccessfullySensed())
	{
		// 액터를 더 이상 감지하지 못함
		if (Actor == m_AggroTarget)
		{
			// 현재 타겟을 잃었으므로 리셋
			m_AggroTarget = nullptr;
		}
	}
	*/
}
