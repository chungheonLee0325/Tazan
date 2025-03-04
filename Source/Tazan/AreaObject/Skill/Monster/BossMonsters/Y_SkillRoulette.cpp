// Fill out your copyright notice in the Description page of Project Settings.
#include "Y_SkillRoulette.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/Utilities/LogMacro.h"


UY_SkillRoulette::UY_SkillRoulette()
{
}

void UY_SkillRoulette::AdjustSkillWeightsByCombatContext(TArray<FSkillRouletteEntry>& LocalEntries)
{
	float dist = Owner->GetDistToTarget();
	FVector dir = Owner->GetDirToTarget();
	dir.Normalize();

	// 거리가 멀면, 원거리 공격 활성화.
	if (dist > LongRange)
	{
		// LOG_SCREEN("원거리 공격 활성화");
		ApplySkillWeight(LocalEntries, EAiSkillType::Long, 400.0f);
	}

	//직전 스킬 타입이 거리벌리기이면, 원거리 공격 위주로 사용
	if (PrevSkillType == EAiSkillType::Back)
	{
		// LOG_SCREEN("원거리 공격!");
		ApplySkillWeight(LocalEntries, EAiSkillType::Long, 1000.0f);
	}

	//직전 스킬 타입이 원거리 공격이면, 거리 벌리기 확률 Down
	else if (PrevSkillType == EAiSkillType::Long)
	{
		ApplySkillWeight(LocalEntries, EAiSkillType::Back, 0.1f);
	}

	//직전 스킬 타입이 좌/우 공격이면, 턴어택 거의 확정 시전
	else if (PrevSkillType == EAiSkillType::Left || PrevSkillType == EAiSkillType::Right)
	{
		SetSkillWeightByID(LocalEntries, 10200, 18.0f);
	}

	// 거리가 짧으면, 위빙 스킬 확률 UP
	else if (dist <= ShortRange)
	{
		ApplySkillWeight(LocalEntries, EAiSkillType::Weaving, 1.5f);
	}

	float forwardDot = FVector::DotProduct(dir, Owner->GetActorForwardVector());
	float rightDot = FVector::DotProduct(dir, Owner->GetActorRightVector());

	// 플레이어가 뒤에 있다면, 뒤도는 스킬 확률 UP
	if (forwardDot < -0.2f)
	{
		SetSkillWeightByID(LocalEntries, 10800, 21.0f);
		SetSkillWeightByID(LocalEntries, 15200, 0.0f);
		if (dist <= ShortRange)
		{
			ApplySkillWeightByID(LocalEntries, 10200, 21.0f);
		}
	}

	if (forwardDot > 0.0f && rightDot >= 0.15f)
	{
		// LOG_PRINT(TEXT("우측 대각선!"));
		if ((PrevSkillType != EAiSkillType::Right) && (PrevSkillType != EAiSkillType::Left))
		{
			SetSkillWeight(LocalEntries, EAiSkillType::Right, 3.0f);
		}
	}
	if (forwardDot > 0.0f && rightDot <= 0.15f)
	{
		// LOG_PRINT(TEXT("좌측 대각선!"));
		if ((PrevSkillType != EAiSkillType::Right) && (PrevSkillType != EAiSkillType::Left))
		{
			SetSkillWeight(LocalEntries, EAiSkillType::Left, 3.0f);
		}
	}
}
