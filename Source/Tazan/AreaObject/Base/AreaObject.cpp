// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaObject.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tazan/AreaObject/Attribute/HealthComponent.h"
#include "Tazan/Contents/TazanGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Attribute/PoiseComponent.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "Tazan/Utilities/LogMacro.h"
#include "Tazan/AreaObject/Attribute/StaminaComponent.h"
#include "Tazan/AreaObject/Utility/RotationComponent.h"
#include "Tazan/Contents/TazanGameMode.h"
#include "Tazan/UI/FloatingDamageActor.h"

// Sets default values
AAreaObject::AAreaObject()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Health Component 생성
	m_HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	// Poise Component 생성
	m_PoiseComponent = CreateDefaultSubobject<UPoiseComponent>(TEXT("PoiseComponent"));

	// Stamina Component 생성
	m_StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));

	// Condition Component 생성
	m_ConditionComponent = CreateDefaultSubobject<UConditionComponent>(TEXT("ConditionComponent"));

	// Rotation Component 생성
	m_RotationComponent = CreateDefaultSubobject<URotationComponent>(TEXT("RotationComponent"));

	//GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
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

	// 데이터 초기화
	UTazanGameInstance* gameInstance = Cast<UTazanGameInstance>(GetGameInstance());
	dt_AreaObject = gameInstance->GetDataAreaObject(m_AreaObjectID);

	// Health 초기화 By Data
	float hpMax = 1.0f;
	int basePoise = 0;
	float maxStamina = 100.0f; // Assuming a default value, actual implementation needed
	float staminaRecoveryRate = 20.f;

	if (dt_AreaObject != nullptr)
	{
		hpMax = dt_AreaObject->HPMax;
		basePoise = dt_AreaObject->BasePoise;
		m_OwnSkillIDSet = dt_AreaObject->SkillList;
		maxStamina = dt_AreaObject->StaminaMax;
		staminaRecoveryRate = dt_AreaObject->StaminaRecoveryRate;
	}

	m_HealthComponent->InitHealth(hpMax);
	m_PoiseComponent->InitPoise(basePoise);
	m_StaminaComponent->InitStamina(maxStamina, staminaRecoveryRate);

	// 스킬 인스턴스 생성
	for (auto& skill : m_OwnSkillIDSet)
	{
		if (FSkillData* skillData = gameInstance->GetDataSkill(skill))
		{
			FString SkillName = FString::Printf(TEXT("BaseSkill_%d"), skill);
			UBaseSkill* NewSkill = NewObject<UBaseSkill>(this, skillData->SkillClass, *SkillName);
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

	// GameMode Setting
	m_GameMode = Cast<ATazanGameMode>(GetWorld()->GetAuthGameMode());
}

void AAreaObject::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called every frame
void AAreaObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_CurrentSkill != nullptr)
	{
		m_CurrentSkill->OnCastTick(DeltaTime);
	}
}

// Called to bind functionality to input
void AAreaObject::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAreaObject::CalcDamage(FAttackData& AttackData, AActor* Caster, AActor* Target, FHitResult& HitInfo)
{
	float Damage = FMath::RandRange(AttackData.HealthDamageAmountMin, AttackData.HealthDamageAmountMax);

	// Apply perfect dodge damage buff if active
	if (bPerfectDodgeBuffActive)
	{
		Damage *= PERFECT_DODGE_DAMAGE_MULTIPLIER;
	}

	if (Target == nullptr)
	{
		return;
	}

	FCustomDamageEvent DamageEvent;
	DamageEvent.AttackData = AttackData;
	DamageEvent.HitInfo = HitInfo;
	DamageEvent.Damage = Damage;

	Target->TakeDamage(Damage, DamageEvent, GetController(), this);
}

float AAreaObject::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator,
                              AActor* DamageCauser)
{
	// ToDo : Can Attack Logic 추가? -> 설인 만들면 추가해야할듯
	if (IsDie() || HasCondition(EConditionBitsType::Invincible))
		return 0.0f;

	float ActualDamage = Damage;

	FHitResult hitResult;
	FVector hitDir;
	FAttackData attackData;
	bool bIsWeakPointHit = false;

	if (DamageEvent.IsOfType(FCustomDamageEvent::ClassID))
	{
		FCustomDamageEvent* const customDamageEvent = (FCustomDamageEvent*)&DamageEvent;
		attackData = customDamageEvent->AttackData;
		customDamageEvent->GetBestHitInfo(this, DamageCauser, hitResult, hitDir);

		// Check for weak point hit
		bIsWeakPointHit = IsWeakPointHit(hitResult.Location);

		// Check for dodge conditions
		if (HasCondition(EConditionBitsType::DodgeWindow) ||
			HasCondition(EConditionBitsType::PerfectDodgeWindow))
		{
			// Complete dodge - no damage
			ActualDamage = 0.0f;

			// Handle perfect dodge
			if (HasCondition(EConditionBitsType::PerfectDodgeWindow))
			{
				HandlePerfectDodge();
			}
			return ActualDamage;
		}

		// Check for guard conditions
		if (HasCondition(EConditionBitsType::Guard) ||
			HasCondition(EConditionBitsType::PerfectGuardWindow))
		{
			ActualDamage = 0.0f;

			if (HasCondition(EConditionBitsType::PerfectGuardWindow))
			{
				HandlePerfectGuard(DamageCauser, attackData);
			}
			else
			{
				HandleGuard(DamageCauser, attackData);
			}
			return ActualDamage;
		}

		// HitStop 처리
		if (attackData.bEnableHitStop)
		{
			// 월드의 시간 스케일을 조절하거나
			// 캐릭터의 애니메이션만 일시 정지
			ApplyHitStop(attackData.HitStopDuration);
		}

		// 넉백 처리
		if (attackData.KnockBackForce > 0.0f)
		{
			FVector knockBackDir;
			if (attackData.bUseCustomKnockBackDirection)
			{
				knockBackDir = attackData.KnockBackDirection;
			}
			else
			{
				// 기본적으로 타격 방향으로 넉백
				knockBackDir = (GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal2D();
			}

			ApplyKnockBack(knockBackDir * attackData.KnockBackForce);
		}

		m_PoiseComponent->PoiseProcess(attackData);
	}

	ActualDamage = Super::TakeDamage(ActualDamage, DamageEvent, EventInstigator, DamageCauser);

	// damage multiply by weakpoint Hit
	ActualDamage = bIsWeakPointHit ? ActualDamage * 1.5f : ActualDamage;

	// apply actual hp damage
	float CurrentHP = DecreaseHP(ActualDamage);
	if (FMath::IsNearlyZero(CurrentHP))
	{
		if (true == ExchangeDead())
		{
			OnDie();
		}
	}

	// apply stamina damage
	DecreaseStamina(attackData.StaminaDamageAmount);

	// Spawn floating damage
	FVector SpawnLocation;
	if (hitResult.Location == FVector::ZeroVector)
	{
		// 캐릭터 중앙에서d 생성
		SpawnLocation = GetActorLocation();
	}
	else
	{
		// Hit 위치에서 생성
		SpawnLocation = hitResult.Location;
	}

	FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

	if (AFloatingDamageActor* DamageActor = GetWorld()->SpawnActor<AFloatingDamageActor>(
		AFloatingDamageActor::StaticClass(), SpawnTransform))
	{
		// FloatingDamageType 계산
		EFloatingDamageType damageType = bIsWeakPointHit
			                                 ? EFloatingDamageType::WeakPointDamage
			                                 : m_DefaultDamageType;
		DamageActor->Initialize(ActualDamage, damageType);
	}

	// Spawn Hit SFX
	if (dt_AreaObject->HitSoundID != 0)
	{
		PlayPositionalSound(dt_AreaObject->HitSoundID, hitResult.Location);
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
	if (false == IsValid(m_CurrentSkill))
	{
		if (m_CurrentSkill != nullptr)
		{
			LOG_PRINT(TEXT("스킬 댕글링 포인터 문제발생!!!!"));
		}
		m_CurrentSkill = nullptr;
		return nullptr;
	}
	return m_CurrentSkill;
}

FAttackData* AAreaObject::GetCurrentSkillAttackData(int Index)
{
	if (false == IsValid(m_CurrentSkill))
	{
		LOG_PRINT(TEXT("스킬 댕글링 포인터 문제발생!!!!"));
		m_CurrentSkill = nullptr;
		return nullptr;
	}
	return m_CurrentSkill->GetAttackDataByIndex(Index);
}

void AAreaObject::UpdateCurrentSkill(UBaseSkill* NewSkill)
{
	if (!IsValid(NewSkill))
	{
		LOG_PRINT(TEXT("스킬 댕글링 포인터 문제발생!!!!"));
		return;
	}

	m_CurrentSkill = NewSkill;
}

UBaseSkill* AAreaObject::GetSkillByID(int SkillID)
{
	auto skillPointer = m_SkillInstanceMap.Find(SkillID);

	if (!IsValid(*skillPointer))
	{
		LOG_PRINT(TEXT("스킬 댕글링 포인터 문제발생!!!!"));
		return nullptr;
	}
	return *skillPointer;
}

bool AAreaObject::CanCastSkill(UBaseSkill* Skill, AAreaObject* Target)
{
	if (nullptr != m_CurrentSkill)
	{
		LOG_PRINT(TEXT("현재 스킬 사용중. m_CurrentSkill 초기화 후 사용"));
		return false;
	}

	if (Skill == nullptr) LOG_PRINT(TEXT("Skill is Empty"));
	if (Target == nullptr) LOG_PRINT(TEXT("Target is Empty"));

	return Skill && Skill->CanCast(this, Target);
}

bool AAreaObject::CanCastNextSkill(UBaseSkill* Skill, AAreaObject* Target)
{
	return bCanNextSkill && Skill && Skill->CanCast(this, Target);
}

bool AAreaObject::CastSkill(UBaseSkill* Skill, AAreaObject* Target)
{
	if (CanCastSkill(Skill, Target))
	{
		UpdateCurrentSkill(Skill);
		Skill->OnCastStart(this, Target);
		return true;
	}
	else
	{
		FString fail = UEnum::GetValueAsString(Skill->SkillFailCase);
		LOG_PRINT(TEXT("CastSkill Failed: %s"), *fail);
		LOG_SCREEN_ERROR(this, "CastSkill Failed");
		return false;
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

void AAreaObject::SetAnimationPoiseBonus(float Bonus) const
{
	m_PoiseComponent->SetAnimationPoiseBonus(Bonus);
}

bool AAreaObject::AddCondition(EConditionBitsType AddConditionType, float Duration)
{
	return m_ConditionComponent->AddCondition(AddConditionType, Duration);
}

bool AAreaObject::RemoveCondition(EConditionBitsType RemoveConditionType) const
{
	return m_ConditionComponent->RemoveCondition(RemoveConditionType);
}

bool AAreaObject::HasCondition(EConditionBitsType HasConditionType) const
{
	return m_ConditionComponent->HasCondition(HasConditionType);
}

bool AAreaObject::ExchangeDead() const
{
	return m_ConditionComponent->ExchangeDead();
}

void AAreaObject::LookAtLocation(const FVector& TargetLocation, EPMRotationMode Mode, float DurationOrSpeed,
                                 float Ratio,
                                 EMovementInterpolationType InterpType)
{
	m_RotationComponent->LookAtLocation(TargetLocation, Mode, DurationOrSpeed, Ratio, InterpType);
}

void AAreaObject::LookAtLocationDirect(const FVector& TargetLocation) const
{
	m_RotationComponent->LookAtLocationDirect(TargetLocation);
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
	return m_HealthComponent->IncreaseHP(Delta);
}

float AAreaObject::DecreaseHP(float Delta) const
{
	return m_HealthComponent->DecreaseHP(Delta);
}

void AAreaObject::SetHPByRate(float Rate) const
{
	m_HealthComponent->SetHPByRate(Rate);
}

float AAreaObject::GetHP() const
{
	return m_HealthComponent->GetHP();
}

float AAreaObject::IncreaseStamina(float Delta) const
{
	return m_StaminaComponent->IncreaseStamina(Delta);
}

float AAreaObject::DecreaseStamina(float Delta, bool bIsDamaged) const
{
	// ToDo : 탈진상태 추가 및 브루탈 어택 변수 셋팅, 탈진 델리게이트 호출 - bIsDamaged == true일때만
	return m_StaminaComponent->DecreaseStamina(Delta);
}

float AAreaObject::GetStamina() const
{
	return m_StaminaComponent->GetStamina();
}

bool AAreaObject::CanUseStamina(float Cost) const
{
	return m_StaminaComponent->CanUseStamina(Cost);
}

void AAreaObject::HandleGuard(AActor* DamageCauser, const FAttackData& Data)
{
	// Regular guard stamina cost
	m_StaminaComponent->DecreaseStamina(Data.StaminaDamageAmount * GUARD_STAMINA_MULTIPLY_RATE);

	// Rotate AreaObject to Damage Causer
	RotateToGuardTarget(DamageCauser->GetActorLocation());

	// Spawn guard VFX
	if (GuardEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			GuardEffect,
			GetActorLocation(), FRotator::ZeroRotator, FVector(2.0f)
		);
	}
	if (GuardSFXID != 0)
	{
		PlayGlobalSound(GuardSFXID);
	}
}

void AAreaObject::HandlePerfectGuard(AActor* DamageCauser, const FAttackData& Data)
{
	// Perfect guard stamina cost
	m_StaminaComponent->DecreaseStamina(Data.StaminaDamageAmount * PERFECT_GUARD_STAMINA_MULTIPLY_RATE);

	// Rotate AreaObject to Damage Causer
	RotateToGuardTarget(DamageCauser->GetActorLocation());

	// Apply stamina damage to attacker
	if (AAreaObject* attacker = Cast<AAreaObject>(DamageCauser))
	{
		attacker->m_StaminaComponent->DecreaseStamina(PERFECT_GUARD_STAMINA_REFLECTION_DAMAGE);
		attacker->AddParryStack();
	}
	// Spawn perfect guard VFX
	if (PerfectGuardEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PerfectGuardEffect,
			GetActorLocation(), FRotator::ZeroRotator, FVector(2.0f)
		);
	}
	if (PerfectGuardSFXID != 0)
	{
		PlayGlobalSound(PerfectGuardSFXID);
	}

	ApplyHitStop(PERFECT_GUARD_HIT_STOP_DURATION);

	// TODO: Could trigger perfect guard animation through montage or notify
}

void AAreaObject::AddParryStack()
{
	++ParryStack;
	// LOG_SCREEN("패리 스택: %d",ParryStack);
	if (ParryStack >= ParryStackMax)
	{
		ParryStackPenalty();
	}
}

void AAreaObject::ParryStackPenalty()
{
	// LOG_SCREEN("패리 패널티!");
	ParryStack = 0;
}

void AAreaObject::HandlePerfectDodge()
{
	// Activate damage buff
	bPerfectDodgeBuffActive = true;

	// Spawn perfect dodge VFX
	if (PerfectDodgeEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PerfectDodgeEffect,
			GetActorLocation()
		);
	}
	if (PerfectDodgeSFXID != 0)
	{
		PlayGlobalSound(PerfectDodgeSFXID);
	}

	// Set timer to remove buff
	FTimerHandle BuffTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		BuffTimerHandle,
		[this]()
		{
			bPerfectDodgeBuffActive = false;
		},
		PERFECT_DODGE_BUFF_DURATION, // Buff duration - could be made configurable
		false
	);

	ApplyHitStop(PERFECT_DODGE_HIT_STOP_DURATION);
}

void AAreaObject::ApplyHitStop(float Duration)
{
	// 월드 전체 시간 조절
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.005f);

	// 타이머로 원래 속도로 복구
	GetWorld()->GetTimerManager().SetTimer(
		HitStopTimerHandle,
		this,
		&AAreaObject::ResetTimeScale,
		Duration * 0.005f, // 실제 시간으로 변환
		false
	);
}

void AAreaObject::ResetTimeScale() const
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

void AAreaObject::ApplyKnockBack(const FVector& KnockBackForce)
{
	// 이미 넉백 중이면 무시
	if (bIsBeingKnockedBack)
		return;

	bIsBeingKnockedBack = true;

	TWeakObjectPtr<AAreaObject> weakThis = this;
	KnockBackCurrentTime = 0.0f;
	AdjustKnockBackForce = KnockBackForce;

	// 넉백 종료 처리
	GetWorld()->GetTimerManager().SetTimer(
		KnockBackTimerHandle,
		[weakThis]()
		{
			AAreaObject* strongThis = weakThis.Get();
			if (strongThis != nullptr)
			{
				strongThis->KnockBackCurrentTime += 0.05f;
				if (strongThis->KnockBackCurrentTime >= strongThis->KnockBackDuration)
				{
					strongThis->bIsBeingKnockedBack = false;
					strongThis->GetWorld()->GetTimerManager().ClearTimer(strongThis->KnockBackTimerHandle);
					return;
				}
				strongThis->AddActorWorldOffset(strongThis->AdjustKnockBackForce * 0.01f,true);
			}
		}, 0.01f, true);
}

void AAreaObject::SetGuardState(bool bIsGuarding)
{
	if (m_StaminaComponent)
	{
		m_StaminaComponent->SetGuardState(bIsGuarding);
	}
}

bool AAreaObject::IsWeakPointHit(const FVector& HitLoc)
{
	return false;
}

void AAreaObject::PlayGlobalSound(int SoundID)
{
	if (m_GameMode == nullptr)
	{
		LOG_PRINT(TEXT("GameMode nullptr"));
	}
	m_GameMode->PlayGlobalSound(SoundID);
}

void AAreaObject::PlayPositionalSound(int SoundID, FVector Position)
{
	if (m_GameMode == nullptr)
	{
		LOG_PRINT(TEXT("GameMode nullptr"));
	}
	m_GameMode->PlayPositionalSound(SoundID, Position);
}

void AAreaObject::PlayBGM(int SoundID, bool bLoop)
{
	if (m_GameMode == nullptr)
	{
		LOG_PRINT(TEXT("GameMode nullptr"));
	}
	m_GameMode->PlayBGM(SoundID, bLoop);
}

void AAreaObject::StopBGM()
{
	if (m_GameMode == nullptr)
	{
		LOG_PRINT(TEXT("GameMode nullptr"));
	}
	m_GameMode->StopBGM();
}

void AAreaObject::RotateToGuardTarget(const FVector& Target)
{
	LookAtLocation(Target, EPMRotationMode::Speed, GUARD_TO_TARGET_ROTATE_SPEED);
}
