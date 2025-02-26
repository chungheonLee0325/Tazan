// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionSkill.h"

#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/Utilities/LogMacro.h"

void UCollisionSkill::OnCastEnd()
{
	Super::OnCastEnd();
	NotifyStateMap.Empty();
}

void UCollisionSkill::CancelCast()
{
	Super::CancelCast();
	NotifyStateMap.Empty();
}

void UCollisionSkill::OnCastTick(float DeltaTime)
{
	Super::OnCastTick(DeltaTime);

	if (m_CurrentPhase != ESkillPhase::PostCasting)
	{
		return;
	}
	for (auto& pair : NotifyStateMap)
	{
		ProcessHitDetection(pair.Key);
	}
}

void UCollisionSkill::SetCasterMesh(int AttackDataIndex, UAnimNotifyState* NotifyState)
{
	// 초기화
	FAttackCollision AttackCollision;
	// LOG_PRINT(TEXT("히트 업데이트"));
	bHasHit = false;

	AttackCollision.IndexedAttackData = GetAttackDataByIndex(AttackDataIndex);
	if (AttackCollision.IndexedAttackData == nullptr)
	{
		return;
	}
	if (AttackCollision.IndexedAttackData->HitBoxData.MeshComponentTag == NAME_None)
	{
		AttackCollision.OwnerSourceMesh = m_Caster->GetMesh();
	}
	// 태그로 지정된 메시 찾기
	TArray<UActorComponent*> Components;
	m_Caster->GetComponents(USkeletalMeshComponent::StaticClass(), Components);

	for (UActorComponent* Component : Components)
	{
		if (Component->ComponentHasTag(AttackCollision.IndexedAttackData->HitBoxData.MeshComponentTag))
		{
			AttackCollision.OwnerSourceMesh = Cast<USkeletalMeshComponent>(Component);
		}
	}
	AttackCollision.IsEnableHitDetection = true;
	NotifyStateMap.Add(NotifyState, AttackCollision);
}

void UCollisionSkill::ProcessHitDetection(UAnimNotifyState* NotifyState)
{
	FAttackCollision* AttackCollision = NotifyStateMap.Find(NotifyState);
	if (!AttackCollision->OwnerSourceMesh || !m_Caster)
		return;

	FVector StartLocation = AttackCollision->OwnerSourceMesh->GetSocketLocation(
		AttackCollision->IndexedAttackData->HitBoxData.StartSocketName);
	FVector EndLocation = AttackCollision->IndexedAttackData->HitBoxData.EndSocketName != NAME_None
		                      ? AttackCollision->OwnerSourceMesh->GetSocketLocation(
			                      AttackCollision->IndexedAttackData->HitBoxData.EndSocketName)
		                      : StartLocation;
	FRotator SocketRotation = AttackCollision->OwnerSourceMesh->GetSocketRotation(
		AttackCollision->IndexedAttackData->HitBoxData.StartSocketName);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(m_Caster);

	TArray<FHitResult> HitResults;
	bool bHit = false;

	switch (AttackCollision->IndexedAttackData->HitBoxData.DetectionType)
	{
	case EHitDetectionType::Line:
		{
			bHit = m_Caster->GetWorld()->LineTraceMultiByChannel(
				HitResults,
				StartLocation,
				EndLocation,
				ECC_GameTraceChannel2,
				QueryParams
			);
			break;
		}
	case EHitDetectionType::Sphere:
		{
			bHit = m_Caster->GetWorld()->SweepMultiByChannel(
				HitResults,
				StartLocation,
				EndLocation,
				FQuat::Identity,
				ECC_GameTraceChannel2,
				FCollisionShape::MakeSphere(AttackCollision->IndexedAttackData->HitBoxData.Radius),
				QueryParams
			);
			break;
		}

	case EHitDetectionType::Capsule:
		{
			FVector location = (StartLocation + EndLocation) / 2.f;
			bHit = m_Caster->GetWorld()->SweepMultiByChannel(
				HitResults,
				location,
				location,
				SocketRotation.Quaternion(),
				ECC_GameTraceChannel2,
				FCollisionShape::MakeCapsule(AttackCollision->IndexedAttackData->HitBoxData.Radius,
				                             AttackCollision->IndexedAttackData->HitBoxData.HalfHeight),
				QueryParams
			);
			break;
		}

	case EHitDetectionType::Box:
		{
			FVector location = (StartLocation + EndLocation) / 2.f;
			bHit = m_Caster->GetWorld()->SweepMultiByChannel(
				HitResults,
				location,
				location,
				SocketRotation.Quaternion(),
				ECC_GameTraceChannel2,
				FCollisionShape::MakeBox(AttackCollision->IndexedAttackData->HitBoxData.BoxExtent),
				QueryParams
			);
			break;
		}
	}

	// 디버그 드로잉
	if (bDebugDraw)
	{
		DrawDebugHitDetection(NotifyState, StartLocation, EndLocation, HitResults, SocketRotation);
	}

	if (!bHit)
	{
		return;
	}

	bHasHit = true;
	// LOG_PRINT(TEXT("트루가 됨"));

	for (FHitResult& Hit : HitResults)
	{
		AActor* hitActor = Hit.GetActor();
		// 이미 히트한 액터는 스킵, AreaObject 말고 BreakableObject도 존재할수 있으므로 별도 처리
		if (hitActor == nullptr || AttackCollision->HitActors.Contains(hitActor))
		{
			continue;
		}
		AttackCollision->HitActors.Add(hitActor);

		AAreaObject* hitAreaObject = Cast<AAreaObject>(Hit.GetActor());
		if (hitAreaObject != nullptr)
		{
			m_Caster->CalcDamage(*AttackCollision->IndexedAttackData, m_Caster, hitActor, Hit);
		}
	}
}

void UCollisionSkill::ResetCollisionData(UAnimNotifyState* NotifyState)
{
	FAttackCollision* AttackCollision = NotifyStateMap.Find(NotifyState);
	AttackCollision->IsEnableHitDetection = false;
	AttackCollision->HitActors.Empty();
	AttackCollision->IndexedAttackData = nullptr;
	AttackCollision->OwnerSourceMesh = nullptr;
	NotifyStateMap.Remove(NotifyState);
}

void UCollisionSkill::DrawDebugHitDetection(UAnimNotifyState* NotifyState, const FVector& Start, const FVector& End,
                                            const TArray<FHitResult>& HitResults, const FRotator& SocketRotation)
{
	FAttackCollision* AttackCollision = NotifyStateMap.Find(NotifyState);
	auto& HitBoxData = AttackCollision->IndexedAttackData->HitBoxData;
	UWorld* World = m_Caster->GetWorld();
	if (!World) return;

	const float Duration = DebugDrawDuration;
	const bool bPersistent = bPersistentLines;
	const FColor TraceColor = DebugColor;
	const FColor HitColor = FColor::Green;

	switch (HitBoxData.DetectionType)
	{
	case EHitDetectionType::Line:
		{
			DrawDebugLine(
				World,
				Start,
				End,
				TraceColor,
				bPersistent,
				Duration,
				0,
				2.0f
			);
			break;
		}

	case EHitDetectionType::Sphere:
		{
			// 시작점과 끝점에 구체 그리기
			DrawDebugSphere(
				World,
				Start,
				HitBoxData.Radius,
				12,
				TraceColor,
				bPersistent,
				Duration
			);

			if (Start != End)
			{
				DrawDebugSphere(
					World,
					End,
					HitBoxData.Radius,
					12,
					TraceColor,
					bPersistent,
					Duration
				);

				// 구체들을 연결하는 라인
				DrawDebugLine(
					World,
					Start,
					End,
					TraceColor,
					bPersistent,
					Duration
				);
			}
			break;
		}

	case EHitDetectionType::Capsule:
		{
			DrawDebugCapsule(
				World,
				(Start + End) * 0.5f,
				HitBoxData.HalfHeight,
				HitBoxData.Radius,
				SocketRotation.Quaternion(),
				TraceColor,
				bPersistent,
				Duration
			);
			break;
		}

	case EHitDetectionType::Box:
		{
			DrawDebugBox(
				World,
				(Start + End) * 0.5f,
				HitBoxData.BoxExtent,
				SocketRotation.Quaternion(),
				TraceColor,
				bPersistent,
				Duration
			);
			break;
		}
	}

	// 히트 포인트 표시
	for (const FHitResult& Hit : HitResults)
	{
		DrawDebugPoint(
			World,
			Hit.ImpactPoint,
			10.0f,
			HitColor,
			bPersistent,
			Duration
		);

		DrawDebugLine(
			World,
			Hit.ImpactPoint,
			Hit.ImpactPoint + Hit.ImpactNormal * 30.0f,
			HitColor,
			bPersistent,
			Duration
		);
	}
}
