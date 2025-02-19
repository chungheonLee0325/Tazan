// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsAttacking = false;
	bCanAttack = true;
	AttackCooldown = 0.5f;
	WeakAttackCurrentComboCount = 0;
	MaxComboCount = 3;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatComponent::AttackInput(int SkillID)
{
	// 공격 중이거나 공격할 수 없는 상태면 입력 무시
	if (false == bCanAttack)
	{
		return;
	}

	if (false == bIsAttacking)
	{
		ExecuteAttack(SkillID);
	}
	else if (true == bCanCombo)
	{
		if (NextComboIndex > CurrentComboCount)
		{
			CurrentComboCount = NextComboIndex;
			ExecuteAttack(SkillID);
		}
	}

}

void UCombatComponent::ExecuteAttack(int SkillID)
{
	bIsAttacking = true;
	bCanAttack = false;

	// 애니메이션 재생 등 실제 공격 로직 구현
	
}

void UCombatComponent::EndAttack()
{
	bCanAttack = true;
	ResetComboState();
}

void UCombatComponent::EnableComboState()
{
	this->bCanCombo = true;
	this->NextComboIndex = FMath::Min(CurrentComboCount + 1, MaxComboCount);
}

void UCombatComponent::ResetComboState()
{
	this->bCanCombo = bCanCombo;
	this->bIsAttacking = false;
}