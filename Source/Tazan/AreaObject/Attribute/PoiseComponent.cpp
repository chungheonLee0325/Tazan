// Fill out your copyright notice in the Description page of Project Settings.
#include "PoiseComponent.h"

#include "Tazan/AreaObject/Base/AreaObject.h"

UPoiseComponent::UPoiseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPoiseComponent::InitPoise(int BasePoise)
{
	m_BasePoise = BasePoise;
}

void UPoiseComponent::BeginPlay()
{
	Super::BeginPlay();

	m_Owner = Cast<AAreaObject>(GetOwner());
}

int32 UPoiseComponent::CalculateTotalDefensePoise() const
{
	int32 TotalModifiers = 0;
	for (const auto& Modifier : ActivePoiseModifiers)
	{
		TotalModifiers += Modifier.Key;
	}
	return FMath::Clamp(m_BasePoise + AnimationPoiseBonus + TotalModifiers, 0, 0xF);
	//return FMath::Max(0, m_BasePoise + AnimationPoiseBonus + TotalModifiers);
}

void UPoiseComponent::PoiseProcess(const FAttackData& AttackData, const FName& Direction)
{
	if (AttackData.StaggerType == EStaggerType::None) return;
	// 우선순위 비교 로직
	EStaggerType staggerType = AttackData.StaggerType;
	const int32 newPriority = GetStaggerPriority(staggerType);
	const int32 currentPriority = GetStaggerPriority(CurrentStagger);
	if (newPriority < currentPriority)
	{
		return;
	}

	// Poise 값 비교 - 공격 vs 방어
	const int32 defensePoise = CalculateTotalDefensePoise();
	if (AttackData.PoiseAttackAmount <= defensePoise)
	{
		return;
	}
	// 스태거 적용
	CurrentStagger = staggerType;
	m_Owner->HandleStaggerBegin(staggerType, Direction);
}

void UPoiseComponent::ClearCurrentStagger()
{
	CurrentStagger = EStaggerType::None;
	//OnStaggerEnd.Broadcast();
}

int32 UPoiseComponent::GetStaggerPriority(EStaggerType Type)
{
	static const TMap<EStaggerType, int32> PriorityMap = {
		{EStaggerType::None, 0},
		{EStaggerType::Weak, 1},
		{EStaggerType::Normal, 2},
		{EStaggerType::Strong, 3},
		{EStaggerType::AirBone, 4},
		{EStaggerType::ParryReaction, 29},
		{EStaggerType::Groggy, 30},
	};
	return PriorityMap.FindRef(Type);
}

void UPoiseComponent::AddPoiseModifier(int32 Value, float Duration)
{
	ActivePoiseModifiers.Add(TPair<int32, float>(Value, Duration));

	FTimerHandle TimerHandle;
	FTimerDelegate Delegate;
	Delegate.BindLambda([this, Value]()
	{
		ActivePoiseModifiers.RemoveAll([Value](const TPair<int32, float>& Elem)
		{
			return Elem.Key == Value;
		});
	});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, Duration, false);
}


void UPoiseComponent::SetAnimationPoiseBonus(int32 Bonus)
{
	AnimationPoiseBonus = FMath::Max(0, Bonus);
}
