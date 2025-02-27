// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player_Kazan.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


class AAreaObject;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TAZAN_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	// 락온 대상 관리
	UPROPERTY()
	AAreaObject* CurrentTarget;

	UPROPERTY()
	TArray<AAreaObject*> PotentialTargets;

	// 락온 설정
	UPROPERTY(EditAnywhere, Category = "Lock On")
	float LockOnRange = 1600.f;

	UPROPERTY(EditAnywhere, Category = "Lock On")
	float LockOnLostRange = 1800.f;

	UPROPERTY(EditAnywhere, Category = "Lock On")
	float MaxTargetAngle = 45.f;

	// 타겟 전환 관련
	bool bIsLockOnMode = false;
	FVector2D LastSwitchInput;
	float SwitchTargetDeadzone = 0.5f;

	// UI 관련 추가
	UPROPERTY()
	//class ULockOnWidget* LockOnWidget;
	class UWidgetComponent* LockOnWidget;
	UPROPERTY()
	AAreaObject* m_Owner;
	//TSubclassOf<ULockOnWidget> WidgetClass;
	APlayerController* m_PlayerController;

	void InitializeLockOnWidget();
	void UpdateLockOnWidget();

public:
	// Sets default values for this component's properties
	ULockOnComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// 기본 메카닉 함수들
	// 토글 On인데 Target 못찾을 경우 false 반환
	bool ToggleLockOn();
	void SwitchTarget(const FVector2D& Direction);
	AAreaObject* FindBestTarget();
	bool IsTargetValid(AAreaObject* Target) const;
	bool IsTargetIsDead(AAreaObject* Target) const;
	bool IsTargetDistanceValid(AAreaObject* Target) const;
	bool IsLockOnMode() { return bIsLockOnMode; }
	AAreaObject* GetCurrentTarget() { return CurrentTarget; };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 타겟 관리 및 검증
	void UpdatePotentialTargets();
	void UpdateCurrentTarget();
	bool IsTargetInRange(AAreaObject* Target) const;
	bool IsTargetVisible(AAreaObject* Target) const;
};
