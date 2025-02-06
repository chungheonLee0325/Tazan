// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpAttack.h"

#include "Tazan/AreaObject/Monster/BaseMonster.h"

UJumpAttack::UJumpAttack()
{
	// ToDo : Skill Param으로 넘겨받기
	SkillData.PrepareTime = 1.0f;
	SkillData.CastTime = 0.9f;
	SkillData.PostCastTime = 0.1f;
	SkillData.Cooldown = 0.0f;
	SkillData.CastRange = 1500.f;

	JumpRange = 800.f;
}

void UJumpAttack::OnPhaseChanged(ESkillPhase NewPhase)
{
	switch (NewPhase)
	{
	case ESkillPhase::Ready:
		{
			break;
		}
	case ESkillPhase::Prepare:
		{
			auto direction = (m_Target->GetActorLocation() - m_Caster->GetActorLocation()).GetSafeNormal2D();
			//m_Caster->LookAtLocation(m_Caster->GetActorLocation() + direction, 0.5f);

			RepeatCount = FMath::RandRange(1,3);
			Radian = 0;
			Amplitue = 0.2f;
			Rotation = FRotator(90.0f, 0.f, 0.f);
			Period = 16.0f;
			break;
		}
	case ESkillPhase::Casting:
		{
			// Set Target Position & Look At Target Position
			auto direction = (m_Target->GetActorLocation() - m_Caster->GetActorLocation()).GetSafeNormal2D();

			// 점프 거리보다 작다면 유도
			if (JumpRange >= FVector::Dist2D(m_Target->GetActorLocation(), m_Caster->GetActorLocation()))
			{
				m_TargetPos = m_Target->GetActorLocation();
				m_TargetPos.Z = m_Caster->GetActorLocation().Z;
			}
			else
			{
				m_TargetPos = m_Caster->GetActorLocation() + direction * JumpRange;
			}

			// Launch
			//m_Caster->MoveToLocation(m_TargetPos, 0.8f);
			//m_Caster->m_VerticalMover->StartVerticalMovement(m_Caster->GetMesh(), 100.f, 0.5f, 0.3f);

			break;
		}
	case ESkillPhase::PostCast:
		{
			auto direction = (m_Target->GetActorLocation() - m_Caster->GetActorLocation()).GetSafeNormal2D();
			//m_Caster->LookAtLocation(m_Caster->GetActorLocation() + direction, 0.5f);
			break;
		}
	}
}

void UJumpAttack::UpdatePreparePhase(float DeltaTime)
{
	Super::UpdatePreparePhase(DeltaTime);
	if (RepeatCount > 0)
	{
		// Animation 
		Radian += DeltaTime * Period;
		if (Radian >= 2 * PI) RepeatCount--;
		Radian = FMath::Fmod(Radian, 2 * PI);
		auto tempR = Rotation * Amplitue * FMath::Sin(Radian);
		tempR.Yaw = m_Caster->GetMesh()->GetRelativeRotation().Yaw;
		tempR.Roll = m_Caster->GetMesh()->GetRelativeRotation().Roll;
		m_Caster->GetMesh()->SetRelativeRotation(tempR);
	}
}

void UJumpAttack::UpdateCastingPhase(float DeltaTime)
{
	Super::UpdateCastingPhase(DeltaTime);
}

void UJumpAttack::UpdatePostCastPhase(float DeltaTime)
{
	Super::UpdatePostCastPhase(DeltaTime);
}
