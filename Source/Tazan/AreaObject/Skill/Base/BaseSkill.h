// BaseSkill.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "BaseSkill.generated.h"

class AAreaObject;

UENUM(BlueprintType)
enum class ESkillPhase : uint8
{
	Ready, // 스킬 사용 가능
	Casting, // 시전 중
	CoolTime // 쿨타임
};

UCLASS()
class TAZAN_API UBaseSkill : public UObject
{
	GENERATED_BODY()

public:
	UBaseSkill();
	// 스킬 초기화 - 데이터 초기화
	void InitSkill(FSkillData* SkillData);

	// 스킬 완료 델리게이트 -> 상태머신에서 사용(다음 상태 전이용)
	DECLARE_DELEGATE(FOnSkillComplete)
	FOnSkillComplete OnSkillComplete;

	// 쿨타임 타이머 핸들
	FTimerHandle CoolTimeTimerHandle;

	virtual bool CanCast(class AAreaObject* Caster, const AAreaObject* Target) const;
	virtual void OnCastStart(class AAreaObject* Caster, AAreaObject* Target);

	// Notify를 이용한 Cast Fire(투사체, 장판 등)
	virtual void OnCastFire();
	// Notify를 이용한 Cast End
	virtual void OnCastEnd();
	// 외부에서 호출을 이용한 스킬 cancel
	virtual void CancelCast();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnMontageBlendOut(UAnimMontage* Montage, bool bInterrupted);

	// Getters
	// 현재 진행 페이즈 반환
	UFUNCTION(BlueprintCallable, Category = "Skill")
	ESkillPhase GetCurrentPhase() const { return m_CurrentPhase; }
	// Attack Data 반환
	FAttackData* GetAttackDataByIndex(int Index) const;

	// 쿨타임 진행률 반환 함수
	UFUNCTION(BlueprintCallable, Category = "Skill")
	float GetCooldownProgress() const;

	const FSkillData* GetSkillData() const { return m_SkillData; }

	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool IsInRange(const AAreaObject* Caster, const AAreaObject* Target) const;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void SkillLogPrint();

protected:
	// 기존 속성들
	UPROPERTY()
	ESkillPhase m_CurrentPhase;

	UPROPERTY()
	AAreaObject* m_Caster;

	UPROPERTY()
	AAreaObject* m_Target;

	UPROPERTY()
	FVector m_TargetPos;

	UPROPERTY()
	TSubclassOf<UBaseSkill> m_NextSkillClass;

	UPROPERTY()
	UBaseSkill* m_NextSkill;

	void AdjustCoolTime();
private:

	FSkillData* m_SkillData;

	float m_CurrentCoolTime;

	int m_SkillID;
};
