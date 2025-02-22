// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Tazan/AreaObject/Attribute/Condition.h"
#include "Tazan/AreaObject/Attribute/Health.h"
#include "Tazan/AreaObject/Attribute/PoiseComponent.h"
#include "Tazan/AreaObject/Utility/RotationComponent.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "Tazan/UI/Widget/FloatingDamageWidget.h"
#include "AreaObject.generated.h"

class URotationComponent;
class ATazanGameMode;
class UCondition;

UCLASS()
class TAZAN_API AAreaObject : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAreaObject();

	UPROPERTY(BlueprintReadWrite)
	UHealth* m_Health;
	UPROPERTY(BlueprintReadWrite)
	UCondition* m_Condition;
	UPROPERTY(BlueprintReadWrite)
	class UStamina* m_Stamina;
	UPROPERTY(BlueprintReadWrite)
	UPoiseComponent* m_PoiseComponent;
	UPROPERTY(BlueprintReadWrite)
	URotationComponent* m_RotationComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AreaObject Data Setting")
	int m_AreaObjectID;
	UPROPERTY(EditDefaultsOnly, Category = "TakeDamage")
	EFloatingDamageType m_DefaultDamageType = EFloatingDamageType::Normal;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 초기화 로직
	virtual void PostInitializeComponents() override;

	// Skill System
	UPROPERTY(EditAnywhere, Category = "Skill")
	TSet<int> m_OwnSkillIDSet;

	UPROPERTY(EditAnywhere, Category = "Skill")
	TMap<int, TObjectPtr<UBaseSkill>> m_SkillInstanceMap;

	UPROPERTY()
	TObjectPtr<UBaseSkill> m_CurrentSkill;

	// Guard/Dodge constants
	const float PERFECT_GUARD_STAMINA_REFLECTION_DAMAGE = 20.0f;
	const float PERFECT_GUARD_STAMINA_MULTIPLY_RATE = 0.8f;
	const float GUARD_STAMINA_MULTIPLY_RATE = 1.2f;
	const float PERFECT_DODGE_BUFF_DURATION = 5.0f;
	const float PERFECT_DODGE_HIT_STOP_DURATION = 0.2f;
	const float PERFECT_GUARD_HIT_STOP_DURATION = 0.2f;
	const float GUARD_TO_TARGET_ROTATE_SPEED = 600.f;
	// Perfect dodge damage multiplier
	const float PERFECT_DODGE_DAMAGE_MULTIPLIER = 1.5f;
	bool bPerfectDodgeBuffActive = false;

	// VFX references
	UPROPERTY(EditDefaultsOnly, Category = "Combat|VFX")
	UParticleSystem* PerfectGuardEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|VFX")
	UParticleSystem* GuardEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|VFX")
	UParticleSystem* PerfectDodgeEffect;

	// SFX references
	UPROPERTY(EditDefaultsOnly, Category = "Combat|SFX")
	int PerfectGuardSFXID;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|SFX")
	int GuardSFXID;
	UPROPERTY(EditDefaultsOnly, Category = "Combat|SFX")
	int PerfectDodgeSFXID;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void CalcDamage(FAttackData& AttackData, AActor* Caster, AActor* Target, FHitResult& HitInfo);
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void OnDie();
	virtual void OnKill();
	virtual void OnRevival();

	bool IsDie() const { return m_Condition->IsDead(); }

	// Health 기능 퍼사드 제공
	UFUNCTION(BlueprintCallable, Category = "HP")
	float IncreaseHP(float Delta) const;
	UFUNCTION(BlueprintCallable, Category = "HP")
	void SetHPByRate(float Rate) const;
	UFUNCTION(BlueprintCallable, Category = "HP")
	float GetHP() const;

	// Stamina 기능 퍼사드 제공
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float IncreaseStamina(float Delta) const;
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float DecreaseStamina(float Delta, bool bIsDamaged = true) const;
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetStamina() const;
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool CanUseStamina(float Cost) const;

	// Condition 기능 퍼사드 제공
	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool AddCondition(EConditionBitsType AddConditionType, float Duration = 0.0f);
	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool RemoveCondition(EConditionBitsType RemoveConditionType) const;
	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool HasCondition(EConditionBitsType HasConditionType) const;
	UFUNCTION(BlueprintCallable, Category = "Condition")
	bool ExchangeDead() const;

	// Rotate 기능 퍼사드 제공
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void LookAtLocation(const FVector& TargetLocation, EPMRotationMode Mode, float DurationOrSpeed,
	                    float Ratio = 1.0f, EMovementInterpolationType InterpType = EMovementInterpolationType::Linear);
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void LookAtLocationDirect(const FVector& TargetLocation) const;

	// Skill Interface
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual UBaseSkill* GetCurrentSkill();
	virtual FAttackData* GetCurrentSkillAttackData(int Index);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual bool CanCastSkill(UBaseSkill* Skill, AAreaObject* Target);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual bool CanCastNextSkill(UBaseSkill* Skill, AAreaObject* Target);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void CastSkill(UBaseSkill* Skill, AAreaObject* Target);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void UpdateCurrentSkill(UBaseSkill* NewSkill);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual UBaseSkill* GetSkillByID(int SkillID);

	// ToDo : 스킬 사용후 이동, 공격 가능 기능 추가 구현
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ClearCurrentSkill();
	virtual void ClearThisCurrentSkill(UBaseSkill* Skill);

	// Stager Interface
	void SetAnimationPoiseBonus(float Bonus) const;

	// Sound Interface 
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayGlobalSound(int SoundID);
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayPositionalSound(int SoundID, FVector Position);
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayBGM(int SoundID, bool bLoop = true);
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopBGM();

	// Stagger 처리 핸들
	UFUNCTION()
	void HandleStaggerBegin(EStaggerType Type, float Duration);
	UFUNCTION()
	void HandleStaggerEnd();
	// ToDo : 종료 Bind 인자 추가?
	void PlayStaggerAnimation(EStaggerType Type) const;
	float GetStaggerAnimationDuration(EStaggerType Type) const;

	// 퍼펙트 가드 처리 핸들
	virtual void HandlePerfectGuard(AActor* DamageCauser, const FAttackData& Data);
	// 가드 처리 핸들
	virtual void HandleGuard(AActor* DamageCauser, const FAttackData& Data);
	// 퍼펙트 회피 처리 핸들
	virtual void HandlePerfectDodge();

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	bool bCanNextSkill = false;

	// Death Setting
	// 죽음 후 destroy 지연 시간
	UPROPERTY(EditDefaultsOnly, Category = "Death Settings")
	float DestroyDelayTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Death Settings")
	UParticleSystem* DeathEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Death Settings")
	FTimerHandle DeathTimerHandle;

	// HitStop 관련
	void ApplyHitStop(float Duration);
	void ResetTimeScale() const;
	FTimerHandle HitStopTimerHandle;

	// 넉백 관련
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyKnockBack(const FVector& KnockbackForce);
	bool bIsBeingKnockedBack = false;

	// 가드 상태 변경 시 호출
	virtual void SetGuardState(bool bIsGuarding);

	//약점에 맞았는지
	virtual bool IsWeakPointHit(const FVector& HitLoc);

	FAreaObjectData* dt_AreaObject;

private:
	UPROPERTY()
	ATazanGameMode* m_GameMode = nullptr;

	void RotateToGuardTarget(const FVector& Target);
};
