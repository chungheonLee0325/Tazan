// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Tazan/AreaObject/Attribute/Condition.h"
#include "Tazan/AreaObject/Attribute/Health.h"
#include "Tazan/AreaObject/Attribute/PoiseComponent.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "AreaObject.generated.h"

class UCondition;

UCLASS()
class TAZAN_API AAreaObject : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAreaObject();
	UPROPERTY(BlueprintReadWrite)
	class UHealth* m_Health;
	UPROPERTY(BlueprintReadWrite)
	UPoiseComponent* m_PoiseComponent;
	UPROPERTY(BlueprintReadWrite)
	UCondition* m_Condition;
	bool bCanNextSkill = false;

	FTimerHandle ConditionTimerHandle;
	EConditionBitsType TempCondition;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 초기화 로직
	virtual void PostInitializeComponents() override;

	// Skill System
	UPROPERTY(EditAnywhere, Category = "Skill")
	TSet<int> m_OwnSkillIDSet;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TMap<int, UBaseSkill*> m_SkillInstanceMap;
	
	UPROPERTY()
	UBaseSkill* m_CurrentSkill;
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

	// Condition 기능 퍼사드 제공
	bool AddCondition(EConditionBitsType AddConditionType, float Duration = 0.0f);
	bool RemoveCondition(EConditionBitsType RemoveConditionType) const;
	bool HasCondition(EConditionBitsType HasConditionType) const;
	bool ExchangeDead() const;

	// Stagger 처리 핸들
	UFUNCTION()
	void HandleStaggerBegin(EStaggerType Type, float Duration);
	UFUNCTION()
	void HandleStaggerEnd();

	// ToDo : 종료 Bind 인자 추가?
	void PlayStaggerAnimation(EStaggerType Type) const;
	float GetStaggerAnimationDuration(EStaggerType Type) const;

	// Health 기능 퍼사드 제공
	float IncreaseHP(float Delta) const;
	void SetHPByRate(float Rate) const;
	float GetHP() const;

	UPROPERTY(EditDefaultsOnly, Category = "Death Settings")
	int m_AreaObjectID;

	FTimerHandle DeathTimerHandle;

	// Death Setting
	// 죽음 후 destroy 지연 시간
	UPROPERTY(EditDefaultsOnly, Category = "Death Settings")
	float DestroyDelayTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Death Settings")
	UParticleSystem* DeathEffect;

private:
	FAreaObjectData* dt_AreaObject;
};
