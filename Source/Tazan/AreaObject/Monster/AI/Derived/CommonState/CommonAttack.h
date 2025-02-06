// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiState.h"
#include "CommonAttack.generated.h"

UENUM()
enum class EAttackPhase : uint8
{
	None,
	Prepare,    // 뒤로 당겨지는 준비 단계
	Attack,     // 돌진 공격 단계
	End         // 공격 종료 단계
};

UCLASS()
class TAZAN_API UCommonAttack : public UBaseAiState
{
	GENERATED_BODY()

public:
	virtual void InitState() override;
	virtual void Enter() override;
	virtual void Execute(float DeltaTime) override;
	virtual void Exit() override;
	void OnSkillCompleted();	

	// 설정 함수들
	void SetM_AttackSpeed(float Speed) { m_AttackSpeed = Speed; }
	void SetM_ChainLength(float Length) { m_ChainLength = Length; }
	void SetM_AnchorPosition(const FVector& Pos) { m_AnchorPosition = Pos; }

protected:
	// 공격 단계 관리
	EAttackPhase m_CurrentPhase = EAttackPhase::None;
	float m_PhaseTimer = 0.0f;
	
	// 공격 관련 변수들
	float m_AttackSpeed = 800.0f;
	float m_ChainLength = 500.0f;
	float m_MinAttackRange = 200.0f;    // 최소 공격 사정거리
	float m_MaxAttackRange = 1000.0f;   // 최대 공격 사정거리
	
	FVector m_AnchorPosition;           // 체인 고정점
	FVector m_AttackStartPosition;      // 공격 시작 위치
	bool m_IsSkillStarted;

	// 각 단계별 처리 함수
	void ExecutePreparePhase(float DeltaTime);
	void ExecuteAttackPhase(float DeltaTime);
	void ExecuteEndPhase(float DeltaTime);
	
	// 거리 체크 함수
	bool IsInAttackRange(const FVector& TargetLocation) const;
};
