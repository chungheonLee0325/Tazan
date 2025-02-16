// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaObject.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tazan/AreaObject/Attribute/Condition.h"
#include "Tazan/AreaObject/Attribute/Health.h"
#include "Tazan/Contents/TazanGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Attribute/PoiseComponent.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/Utilities/LogMacro.h"

// Sets default values
AAreaObject::AAreaObject()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Health Component 생성
	m_Health = CreateDefaultSubobject<UHealth>(TEXT("Health"));

	// Poise Component 생성
	m_PoiseComponent = CreateDefaultSubobject<UPoiseComponent>(TEXT("PoiseComponent"));
}


// Called when the game starts or when spawned
void AAreaObject::BeginPlay()
{
	Super::BeginPlay();

	// AreaObject ID 는 반드시 셋팅되어야 함!!
	if (m_AreaObjectID == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Area Object ID is 0!!!"));
		return;
	}

	// Condition Component 생성
	m_Condition = NewObject<UCondition>(this, UCondition::StaticClass());

	// 데이터 초기화
	UTazanGameInstance* gameInstance = Cast<UTazanGameInstance>(GetGameInstance());
	dt_AreaObject = gameInstance->GetDataAreaObject(m_AreaObjectID);

	// Health 초기화 By Data
	float hpMax = 1.0f;
	int basePoise = 0;

	if (dt_AreaObject != nullptr)
	{
		hpMax = dt_AreaObject->HPMax;
		basePoise = dt_AreaObject->BasePoise;
		m_OwnSkillIDSet = dt_AreaObject->SkillList;
	}

	m_Health->InitHealth(hpMax);
	m_PoiseComponent->InitPoise(basePoise);

	// 스킬 인스턴스 생성
	for (auto& skill : m_OwnSkillIDSet)
	{
		if (FSkillData* skillData = gameInstance->GetDataSkill(skill))
		{
			UBaseSkill* NewSkill = NewObject<UBaseSkill>(this, skillData->SkillClass);
			NewSkill->InitSkill(skillData);
			m_SkillInstanceMap.Add(skill, NewSkill);
		}
		else
		{
			LOG_SCREEN_MY(4.0f, FColor::Red, "%d 해당 아이디의 스킬이 존재하지 않습니다.", skill);
			UE_LOG(LogTemp, Error, TEXT("Skill ID is 0!!!"));
		}
	}

	// 스태거 적용 바인드
	m_PoiseComponent->OnStaggerBegin.AddDynamic(this, &AAreaObject::HandleStaggerBegin);
	m_PoiseComponent->OnStaggerEnd.AddDynamic(this, &AAreaObject::HandleStaggerEnd);
}

void AAreaObject::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called every frame
void AAreaObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAreaObject::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAreaObject::CalcDamage(FAttackData& AttackData, AActor* Caster, AActor* Target, FHitResult& HitInfo)
{
	float Damage = FMath::RandRange(AttackData.HealthDamageAmountMin, AttackData.HealthDamageAmountMax);
	if (Target == nullptr || (Damage != 0.f))
	{
		return;
	}

	// UGameplayStatics::ApplyDamage(Target,
	//                              Damage,
	//                              GetController(),
	//                              this,
	//                              UDamageType::StaticClass());

	FCustomDamageEvent DamageEvent;
	DamageEvent.AttackData = AttackData;
	DamageEvent.HitInfo = HitInfo;
	DamageEvent.Damage = Damage;

	Target->TakeDamage(Damage, DamageEvent, GetController(), this);
}

float AAreaObject::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator,
                              AActor* DamageCauser)
{
	if (IsDie() || HasCondition(EConditionBitsType::Invincible))
		return 0.0f;

	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.IsOfType(FCustomDamageEvent::ClassID))
	{
		// point damage event, pass off to helper function
		FCustomDamageEvent* const customDamageEvent = (FCustomDamageEvent*) &DamageEvent;
		m_PoiseComponent->PoiseProcess(customDamageEvent->AttackData);
	}
		
	if (FMath::IsNearlyZero(IncreaseHP(-ActualDamage)))
	{
		if (true == ExchangeDead())
		{
			OnDie();
		}
	}

	return ActualDamage;
}

void AAreaObject::OnDie()
{
	if (UAnimMontage* montage = dt_AreaObject->Die_AnimMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(montage);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TWeakObjectPtr<AAreaObject> weakThis = this;
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, [weakThis]()
	{
		AAreaObject* strongThis = weakThis.Get(); // 콜리전 전환

		if (strongThis != nullptr)
		{
			// Death Effect
			if (strongThis->DeathEffect != nullptr)
			{
				UGameplayStatics::SpawnEmitterAtLocation(strongThis->GetWorld(), strongThis->DeathEffect,
				                                         strongThis->GetActorLocation());
			}
			strongThis->Destroy();
		}
	}, DestroyDelayTime, false);
}

void AAreaObject::OnKill()
{
}

void AAreaObject::OnRevival()
{
}

UBaseSkill* AAreaObject::GetCurrentSkill()
{
	return m_CurrentSkill;
}

FAttackData* AAreaObject::GetCurrentSkillAttackData(int Index)
{
	return m_CurrentSkill->GetAttackDataByIndex(Index);
}

void AAreaObject::UpdateCurrentSkill(UBaseSkill* NewSkill)
{
	if (nullptr == NewSkill)
	{
		return;
	}

	m_CurrentSkill = NewSkill;
}

UBaseSkill* AAreaObject::GetSkillByID(int SkillID)
{
	UBaseSkill** skillPointer = m_SkillInstanceMap.Find(SkillID);

	if (skillPointer == nullptr)
	{
		return nullptr;
	}
	return *skillPointer;
}

bool AAreaObject::CanCastSkill(UBaseSkill* Skill, AAreaObject* Target)
{
	// ToDo : Cost 소모 확인
	if (Skill == nullptr) LOG_PRINT(TEXT("Skill is Empty"));
	if (Target == nullptr) LOG_PRINT(TEXT("Target is Empty"));

	return Skill && Skill->CanCast(this, Target);
}

bool AAreaObject::CanCastNextSkill(UBaseSkill* Skill, AAreaObject* Target)
{
	return bCanNextSkill && Skill && Skill->CanCast(this, Target);
}

void AAreaObject::CastSkill(UBaseSkill* Skill, AAreaObject* Target)
{
	if (CanCastSkill(Skill, Target))
	{
		UpdateCurrentSkill(Skill);
		Skill->OnCastStart(this, Target);
	}
	else
	{
		LOG_SCREEN_ERROR(this, "CastSkill Failed");
	}
}

void AAreaObject::ClearCurrentSkill()
{
	m_CurrentSkill = nullptr;
}

void AAreaObject::ClearThisCurrentSkill(UBaseSkill* Skill)
{
	if (m_CurrentSkill == Skill)
	{
		m_CurrentSkill = nullptr;
	}
}

bool AAreaObject::AddCondition(EConditionBitsType Condition) const
{
	return m_Condition->AddCondition(Condition);
}

bool AAreaObject::RemoveCondition(EConditionBitsType Condition) const
{
	return m_Condition->RemoveCondition(Condition);
}

bool AAreaObject::HasCondition(EConditionBitsType Condition) const
{
	return m_Condition->HasCondition(Condition);
}

bool AAreaObject::ExchangeDead() const
{
	return m_Condition->ExchangeDead();
}

void AAreaObject::HandleStaggerBegin(EStaggerType Type, float Duration) 
{
	// 애니메이션 재생
	PlayStaggerAnimation(Type);

	// 이동 불가
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	// ToDo : 스킬 사용 불가
}

void AAreaObject::HandleStaggerEnd() 
{
	// 이동 불가 해제
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	// ToDo : 스킬 사용 불가 해제
}

float AAreaObject::GetStaggerAnimationDuration(EStaggerType Type) const
{
	// 실제 애니메이션 데이터에서 길이 조회
	if (UAnimMontage** montage = dt_AreaObject->Stagger_AnimMontages.Find(Type))
	{
		if (*montage) // Valid check for UAnimMontage*
		{
			return (*montage)->GetPlayLength();
		}
	}

	// 애니메이션이 없거나 유효하지 않을 경우 기본값 반환
	UE_LOG(LogTemp, Warning, TEXT("GetStaggerAnimationDuration: Montage for %d is null"), (int32)Type);
	return 0.0f;
}

void AAreaObject::PlayStaggerAnimation(EStaggerType Type) const
{
	if (UAnimMontage** montage = dt_AreaObject->Stagger_AnimMontages.Find(Type))
	{
		if (*montage) // Valid check for UAnimMontage*
		{
			GetMesh()->GetAnimInstance()->Montage_Play(*montage);
			// Todo : 종료 바인드?
		}
	}
	else
	{
		// 애니메이션이 없거나 유효하지 않을 경우 기본값 반환
		UE_LOG(LogTemp, Warning, TEXT("GetStaggerAnimationDuration: Montage for %d is null"), (int32)Type);
	}
}

float AAreaObject::IncreaseHP(float Delta) const
{
	return m_Health->IncreaseHP(Delta);
}

void AAreaObject::SetHPByRate(float Rate) const
{
	m_Health->SetHPByRate(Rate);
}

float AAreaObject::GetHP() const
{
	return m_Health->GetHP();
}
