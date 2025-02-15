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
	//auto player = Cast<APlayer_Kazan>(OtherActor);
	//if (player != nullptr)
	//{
	//	CalcDamage(1.0f, this, player);
	//}
}

bool ABaseMonster::IsMoving() const
{
	return true;
}

bool ABaseMonster::IsRotating() const
{
	return true;
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

AActor* ABaseMonster::GetAggroTarget() const
{
	return m_AggroTarget;
}


void ABaseMonster::StopAll()
{
}
