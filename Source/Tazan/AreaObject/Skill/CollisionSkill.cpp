// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionSkill.h"

#include "Tazan/AreaObject/Base/AreaObject.h"

void UCollisionSkill::OnCastEnd()
{
	Super::OnCastEnd();
	ResetCollisionData();
}

void UCollisionSkill::CancelCast()
{
	Super::CancelCast();
	ResetCollisionData();
}

void UCollisionSkill::OnCastTick(float DeltaTime)
{
	Super::OnCastTick(DeltaTime);

	if (m_CurrentPhase != ESkillPhase::PostCasting || IsEnableHitDetection == false)
	{
		return;
	}
	
	ProcessHitDetection();
}

void UCollisionSkill::SetCasterMesh(int AttackDataIndex)
{
	// 초기화
	ResetCollisionData();
	
	IndexedAttackData = GetAttackDataByIndex(AttackDataIndex);
	if (IndexedAttackData == nullptr)
	{
		return;
	}
	if (IndexedAttackData->HitBoxData.MeshComponentTag == NAME_None)
	{
		OwnerSourceMesh = m_Caster->GetMesh();
	}
	// 태그로 지정된 메시 찾기
	TArray<UActorComponent*> Components;
	m_Caster->GetComponents(USkeletalMeshComponent::StaticClass(), Components);

	for (UActorComponent* Component : Components)
	{
		if (Component->ComponentHasTag(IndexedAttackData->HitBoxData.MeshComponentTag))
		{
			OwnerSourceMesh = Cast<USkeletalMeshComponent>(Component);
		}
	}
	IsEnableHitDetection = true;
}

void UCollisionSkill::ProcessHitDetection()
{
	if (!OwnerSourceMesh || !m_Caster)
		return;

	FVector StartLocation = OwnerSourceMesh->GetSocketLocation(IndexedAttackData->HitBoxData.StartSocketName);
	FVector EndLocation = IndexedAttackData->HitBoxData.EndSocketName != NAME_None
		                      ? OwnerSourceMesh->GetSocketLocation(IndexedAttackData->HitBoxData.EndSocketName)
		                      : StartLocation;
	FRotator SocketRotation = OwnerSourceMesh->GetSocketRotation(IndexedAttackData->HitBoxData.StartSocketName);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(m_Caster);

	TArray<FHitResult> HitResults;
	bool bHit = false;

	switch (IndexedAttackData->HitBoxData.DetectionType)
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
				FCollisionShape::MakeSphere(IndexedAttackData->HitBoxData.Radius),
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
				FCollisionShape::MakeCapsule(IndexedAttackData->HitBoxData.Radius,
				                             IndexedAttackData->HitBoxData.HalfHeight),
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
				FCollisionShape::MakeBox(IndexedAttackData->HitBoxData.BoxExtent),
				QueryParams
			);
			break;
		}
	}

	// 디버그 드로잉
	if (bDebugDraw)
	{
		DrawDebugHitDetection(StartLocation, EndLocation, HitResults, SocketRotation);
	}

	if (!bHit)
	{
		return;
	}

	for (FHitResult& Hit : HitResults)
	{
		AActor* hitActor = Hit.GetActor();
		// 이미 히트한 액터는 스킵, AreaObject 말고 BreakableObject도 존재할수 있으므로 별도 처리
		if (hitActor == nullptr || HitActors.Contains(hitActor))
		{
			continue;
		}
		HitActors.Add(hitActor);

		AAreaObject* hitAreaObject = Cast<AAreaObject>(Hit.GetActor());
		if (hitAreaObject != nullptr)
		{
			m_Caster->CalcDamage(*IndexedAttackData, m_Caster, hitActor, Hit);
		}
	}
}

void UCollisionSkill::ResetCollisionData()
{
	IsEnableHitDetection = false;
	HitActors.Empty();
	IndexedAttackData = nullptr;
	OwnerSourceMesh = nullptr;
}

void UCollisionSkill::DrawDebugHitDetection(const FVector& Start, const FVector& End,
                                            const TArray<FHitResult>& HitResults, const FRotator& SocketRotation) const
{
	auto& HitBoxData = IndexedAttackData->HitBoxData;
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
