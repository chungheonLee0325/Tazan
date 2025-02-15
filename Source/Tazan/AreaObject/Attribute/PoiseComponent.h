// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "Containers/Map.h"
#include "PoiseComponent.generated.h"

class AAreaObject;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaggerBeginSignature, EStaggerType, StaggerType, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaggerEndSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TAZAN_API UPoiseComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPoiseComponent();

	void InitPoise(int BasePoise);
	
	virtual void BeginPlay() override;

	// 강인도 계산
	UFUNCTION(BlueprintCallable)
	int32 CalculateTotalDefensePoise() const;

	// 공격 처리
	UFUNCTION(BlueprintCallable)
	void ProcessAttack(const FAttackData& AttackData);

	// 강인도 수정자 추가
	UFUNCTION(BlueprintCallable)
	void AddPoiseModifier(int32 Value, float Duration);

	// 애니메이션 강인도 설정
	UFUNCTION(BlueprintCallable)
	void SetAnimationPoiseBonus(int32 Bonus, float Duration);

	// 스태거 실제 적용
	UFUNCTION(BlueprintCallable)
	void ApplyStagger(EStaggerType Type, float AnimationDuration);

	UPROPERTY(BlueprintAssignable)
	FOnStaggerBeginSignature OnStaggerBegin;

	UPROPERTY(BlueprintAssignable)
	FOnStaggerEndSignature OnStaggerEnd;
	

protected:
	// 기본 강인도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Poise")
	int32 m_BasePoise = 0;

	// 애니메이션 강인도 보너스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Poise")
	int32 AnimationPoiseBonus = 0;

	// 강인도 수정자
	TArray<TPair<int32, float>> ActivePoiseModifiers;

	// 현재 스태거 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Poise")
	EStaggerType CurrentStagger = EStaggerType::None;

private:
	void ClearStagger();
	static int32 GetStaggerPriority(EStaggerType Type);

	UPROPERTY()
	AAreaObject* m_Owner;
	FTimerHandle StaggerTimerHandle;
};
