// BaseSkill.cpp

#include "BaseSkill.h"

#include "Chaos/Deformable/MuscleActivationConstraints.h"
#include "Tazan/Animation/Player/KazanAniminstance.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/Contents/TazanGameInstance.h"

UBaseSkill::UBaseSkill()
{
	m_CurrentPhase = ESkillPhase::Ready;
	m_CurrentCoolTime = 0.0f;
	m_Caster = nullptr;
	m_Target = nullptr;
}

void UBaseSkill::InitSkill(FSkillData* SkillData)
{
	m_SkillData = SkillData;
}

bool UBaseSkill::CanCast(AAreaObject* Caster, const AAreaObject* Target) const
{
	if (!Caster || !Target) return false;

	// 스킬 상태 체크
	if (m_CurrentPhase != ESkillPhase::Ready) return false;
	if (m_CurrentCoolTime > 0.0f) return false;

	// 사거리 체크
	return IsInRange(Caster, Target);
}

void UBaseSkill::OnCastStart(AAreaObject* Caster, AAreaObject* Target)
{
	if (!Caster || !Target) return;

	m_Caster = Caster;
	m_Target = Target;

	m_CurrentPhase = ESkillPhase::Casting;

	
	// 애니메이션 몽타주 재생
	UAnimInstance* AnimInstance = Caster->GetMesh()->GetAnimInstance();
	if (AnimInstance && m_SkillData->Montage)
	{
		// 기존 델리게이트 해제
		//AnimInstance->Montage_SetEndDelegate(nullptr, m_SkillData->Montage);

		// ToDo :: @@LCH 델리게이트 동작 안함!!! 1순위
		// 델리게이트 바인딩
		//FOnMontageEnded EndDelegate;
		//EndDelegate.BindUObject(this, &UBaseSkill::OnMontageEnded);
		//AnimInstance->Montage_SetEndDelegate(EndDelegate, m_SkillData->Montage);
		//
		//// 블렌드 아웃
		//FOnMontageBlendingOutStarted CompleteDelegate;
		//CompleteDelegate.BindUObject(this, &UBaseSkill::OnMontageBlendOut);
		//AnimInstance->Montage_SetBlendingOutDelegate(CompleteDelegate, m_SkillData->Montage);
        
		Caster->PlayAnimMontage(m_SkillData->Montage);
	}
}

void UBaseSkill::OnCastEnd()
{
	if (!m_Caster || !m_Target) return;

	m_Caster->ClearCurrentSkill();
	if (nullptr != m_NextSkill && m_Caster->CanCastNextSkill(m_NextSkill, m_Target))
	{
		m_NextSkill->OnSkillComplete = OnSkillComplete;
		m_Caster->CastSkill(m_NextSkill, m_Target);
	}
	else
	{
		if (OnSkillComplete.IsBound() == true)
		{
			OnSkillComplete.Execute();
			OnSkillComplete.Unbind();
		}
	}
	m_CurrentPhase = ESkillPhase::CoolTime;
	AdjustCoolTime();
}

void UBaseSkill::CancelCast()
{
	if (!m_Caster) return;
	if (OnSkillComplete.IsBound() == true)
	{
		OnSkillComplete.Unbind();
	}
	if (m_CurrentPhase != ESkillPhase::CoolTime)
	{
		m_Caster->ClearCurrentSkill();
		m_CurrentPhase = ESkillPhase::CoolTime;
		AdjustCoolTime();
	}
}

void UBaseSkill::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == m_SkillData->Montage)
	{
		if (bInterrupted)
		{
			CancelCast();
		}
		else
		{
			OnCastEnd();
		}
	}
}

void UBaseSkill::OnMontageBlendOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == m_SkillData->Montage)
	{
		if (bInterrupted)
		{
			CancelCast();
		}
		else
		{
			OnCastEnd();
		}
	}
}

float UBaseSkill::GetCooldownProgress() const
{
	if (m_SkillData->CoolTime <= 0.0f) return 1.0f;
	return 1.0f - (m_CurrentCoolTime / m_SkillData->CoolTime);
}

bool UBaseSkill::IsInRange(const AAreaObject* Caster, const AAreaObject* Target) const
{
	if (!Caster || !Target) return false;

	float DistanceSquared = FVector::DistSquared(Caster->GetActorLocation(), Target->GetActorLocation());
	float RangeSquared = m_SkillData->CastRange * m_SkillData->CastRange;

	return DistanceSquared <= RangeSquared;
}

void UBaseSkill::AdjustCoolTime()
{
	// 쿨타임 없는 스킬은 바로 준비 완료
	m_CurrentCoolTime = m_SkillData->CoolTime;
	if (FMath::IsNearlyZero(m_CurrentCoolTime))
	{
		m_CurrentPhase = ESkillPhase::Ready;
		return;
	}

	// 쿨타임 있는 스킬은 쿨타임 로직
	TWeakObjectPtr<UBaseSkill> WeakThis = this;

	GetWorld()->GetTimerManager().SetTimer(CoolTimeTimerHandle, [WeakThis]
	{
		UBaseSkill* StrongThis = WeakThis.Get();
		if (StrongThis != nullptr)
		{
			StrongThis->m_CurrentCoolTime = FMath::Max(0.f, StrongThis->m_CurrentCoolTime - 0.1f);
			if (FMath::IsNearlyZero(StrongThis->m_CurrentCoolTime))
			{
				StrongThis->GetWorld()->GetTimerManager().ClearTimer(StrongThis->CoolTimeTimerHandle);
				StrongThis->m_CurrentPhase = ESkillPhase::Ready;
				// ToDo : 쿨타임 완료 이벤트 바인딩?
			}
		}
	}, 0.1f, true);
}
