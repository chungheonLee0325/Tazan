// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMonster.h"

#include "AI/Base/BaseAiFSM.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABaseMonster::ABaseMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

	m_SpawnLocation = GetActorLocation();

	// 스킬 인스턴스 생성
	for (auto& Pair : m_StateSkillClasses)
	{
		if (Pair.Value)
		{
			UBaseSkill* NewSkill = NewObject<UBaseSkill>(this, Pair.Value);
			if (NewSkill)
			{
				m_StateSkillInstances.Add(Pair.Key, NewSkill);
				m_SkillInstances.Add(Pair.Value, NewSkill);
			}
		}
	}
	for (auto& skillClass : m_SkillClasses)
	{
		if (skillClass)
		{
			UBaseSkill* NewSkill = NewObject<UBaseSkill>(this, skillClass);
			m_SkillInstances.Add(skillClass, NewSkill);
		}
	}
	if (m_AiFSM != nullptr)
	{
		m_AiFSM->InitStatePool();
	}
}

// Called every frame
void ABaseMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 스킬 업데이트
	if (m_CurrentSkill)
	{
		m_CurrentSkill->OnCastTick(DeltaTime);
	}
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
	//auto player = Cast<APlayer_Kazan>(OtherActor);
	//if (player != nullptr)
	//{
	//	CalcDamage(1.0f, this, player);
	//}
}

UBaseSkill* ABaseMonster::FindSkillByState(EAiStateType StateType) const
{
	if (UBaseSkill* const* SkillPtr = m_StateSkillInstances.Find(StateType))
	{
		return *SkillPtr;
	}
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

UBaseSkill* ABaseMonster::FindSkillByClass(TSubclassOf<UBaseSkill> SkillClass)
{
	auto skill = m_SkillInstances.Find(SkillClass);
	if (skill != nullptr)
	{
		return *skill;
	}
	return nullptr;
}

void ABaseMonster::OnDie()
{
	Super::OnDie();

	// FSM 정지
	m_AiFSM->StopFSM();
	// Skill 정지
	if (nullptr != m_CurrentSkill) m_CurrentSkill->CancelCast();
	// 움직임 정지
	StopAll();
	// 콜리전 전환
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TWeakObjectPtr<ABaseMonster> WeakThis = this;

	// Delay 후 폭발
	GetWorld()->GetTimerManager().SetTimer(OnDieHandle, [WeakThis]()
	{
		ABaseMonster* StrongThis = WeakThis.Get();
		// Death Effect
		UGameplayStatics::SpawnEmitterAtLocation(StrongThis->GetWorld(), StrongThis->DeathEffect,
		                                         StrongThis->GetActorLocation());
		if (nullptr != StrongThis)
		{
			StrongThis->Destroy();
		}
	}, DestroyDelayTime, false);
}

UBaseAiFSM* ABaseMonster::CreateFSM()
{
	return nullptr;
}

void ABaseMonster::StopRotating()
{
}

void ABaseMonster::StopMoving()
{
}

UBaseSkill* ABaseMonster::GetCurrentSkill()
{
	return m_CurrentSkill;
}

void ABaseMonster::UpdateCurrentSkill(UBaseSkill* NewSkill)
{
	if (nullptr == NewSkill)
	{
		NewSkill = DeQueueSkill();
	}

	if (nullptr == NewSkill)
	{
		return;
	}

	m_CurrentSkill = NewSkill;
}

void ABaseMonster::ClearCurrentSkill()
{
	m_CurrentSkill = nullptr;
}

AActor* ABaseMonster::GetAggroTarget() const
{
	return m_AggroTarget;
}

bool ABaseMonster::CanCastSkill(UBaseSkill* Skill, const AActor* Target) const
{
	return Skill && Skill->CanCast(const_cast<ABaseMonster*>(this), Target);
}

void ABaseMonster::CastSkill(UBaseSkill* Skill, const AActor* Target)
{
	if (CanCastSkill(Skill, Target))
	{
		UpdateCurrentSkill(Skill);
		Skill->OnCastStart(this, Target);
	}
}

void ABaseMonster::EnQueueSkill(UBaseSkill* Skill)
{
	m_SkillQueue.Enqueue(Skill);
}

UBaseSkill* ABaseMonster::DeQueueSkill()
{
	UBaseSkill* Skill = nullptr;
	m_SkillQueue.Dequeue(Skill);

	return Skill;
}

void ABaseMonster::ClearQueueSkill()
{
	m_SkillQueue.Empty();
}

void ABaseMonster::StopAll()
{
}
