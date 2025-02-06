// BaseSkill.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"
#include "BaseSkill.generated.h"

UENUM(BlueprintType)
enum class ESkillPhase : uint8
{
	Ready, // 스킬 사용 가능
	Prepare, // 선딜레이
	Casting, // 시전 중
	PostCast, // 후딜레이
	//Cooldown    // 쿨타임
};

USTRUCT(BlueprintType)
struct FSkillData
{
	GENERATED_BODY()

	// 기존 데이터 유지
	UPROPERTY(EditAnywhere)
	float CastRange = 200.0f;

	UPROPERTY(EditAnywhere)
	float PrepareTime = 0.0f;

	UPROPERTY(EditAnywhere)
	float CastTime = 0.0f;

	UPROPERTY(EditAnywhere)
	float PostCastTime = 0.5f;

	UPROPERTY(EditAnywhere)
	float Cooldown = 1.0f;

	// 이펙트 관련 데이터 추가
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* CastEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* ProgressEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* EndEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* CastNiagaraEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* ProgressNiagaraEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* EndNiagaraEffect = nullptr;
};

UCLASS(Abstract, BlueprintType)
class TAZAN_API UBaseSkill : public UObject
{
	GENERATED_BODY()

public:
	UBaseSkill();

	DECLARE_DELEGATE(FOnSkillComplete)
	FOnSkillComplete OnSkillComplete;

	virtual bool CanCast(class ABaseMonster* Caster, const AActor* Target) const;
	virtual void OnCastStart(class ABaseMonster* Caster, const AActor* Target);
	virtual void OnCastTick(float DeltaTime);
	virtual void UpdatePreparePhase(float DeltaTime);
	virtual void UpdateCastingPhase(float DeltaTime);
	virtual void UpdatePostCastPhase(float DeltaTime);
	virtual void OnCastEnd();
	virtual void CancelCast();

	// Getters
	UFUNCTION(BlueprintCallable, Category = "Skill")
	ESkillPhase GetCurrentPhase() const { return m_CurrentPhase; }

	UFUNCTION(BlueprintCallable, Category = "Skill")
	float GetPhaseProgress() const;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	float GetCooldownProgress() const;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	const FSkillData& GetSkillData() const { return SkillData; }

	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool IsInRange(const ABaseMonster* Caster, const AActor* Target) const;

	// Setters
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void Set_TargetPos(const FVector& M_LaunchPosition);
protected:
	// 기존 속성들
	UPROPERTY(EditAnywhere, Category = "Skill Data")
	FSkillData SkillData;

	UPROPERTY()
	ESkillPhase m_CurrentPhase;

	UPROPERTY()
	float m_CurrentPhaseTime;

	UPROPERTY()
	float m_CurrentCooldown;

	UPROPERTY()
	class ABaseMonster* m_Caster;

	UPROPERTY()
	const AActor* m_Target;

	UPROPERTY()
	FVector m_TargetPos;

	UPROPERTY()
	TSubclassOf<UBaseSkill> m_NextSkillClass;
	
	UPROPERTY()
	UBaseSkill* m_NextSkill;

	// 이펙트 관련 속성 추가
	UPROPERTY()
	TArray<UParticleSystemComponent*> ActiveParticleEffects;

	UPROPERTY()
	TArray<UNiagaraComponent*> ActiveNiagaraEffects;

	// 이펙트 관련 메서드
	virtual void SpawnCastEffect();
	virtual void SpawnProgressEffect();
	virtual void SpawnEndEffect();
	virtual void ClearEffects();

	// 유틸리티 메서드
	virtual void OnPhaseChanged(ESkillPhase NewPhase);
	void UpdatePhase(ESkillPhase NewPhase);
	USceneComponent* GetAttachComponent() const;
};
