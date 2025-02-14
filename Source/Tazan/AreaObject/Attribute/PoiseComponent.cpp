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

void UPoiseComponent::ProcessAttack(const FAttackData& AttackData)
{
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
	if (AttackData.PoiseBreakAmount <= defensePoise)
	{
		return;
	}

	float animationDuration = m_Owner->GetStaggerAnimationDuration(staggerType);
	// 스태거 적용
	ApplyStagger(staggerType, animationDuration);
}

void UPoiseComponent::ApplyStagger(EStaggerType Type, float AnimationDuration)
{
	// 기존 스태거 정리
	if (CurrentStagger != EStaggerType::None)
	{
		GetWorld()->GetTimerManager().ClearTimer(StaggerTimerHandle);
		OnStaggerEnd.Broadcast();
	}

	CurrentStagger = Type;

	// 애니메이션 길이로 타이머 설정
	if (AnimationDuration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			StaggerTimerHandle,
			this,
			&UPoiseComponent::ClearStagger,
			AnimationDuration,
			false
		);
	}

	// 스태거 타입과 필요한 정보만 전달
	OnStaggerBegin.Broadcast(Type, AnimationDuration);
}

void UPoiseComponent::ClearStagger()
{
	CurrentStagger = EStaggerType::None;
	OnStaggerEnd.Broadcast();
}

int32 UPoiseComponent::GetStaggerPriority(EStaggerType Type)
{
	static const TMap<EStaggerType, int32> PriorityMap = {
		{EStaggerType::None, 0},
		{EStaggerType::Weak, 1},
		{EStaggerType::Normal, 2},
		{EStaggerType::Strong, 3},
		{EStaggerType::AirBone, 4}
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
