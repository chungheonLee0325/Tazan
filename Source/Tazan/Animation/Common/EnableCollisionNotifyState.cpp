// Fill out your copyright notice in the Description page of Project Settings.


#include "EnableCollisionNotifyState.h"

#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/AreaObject/Skill/CollisionSkill.h"
#include "Tazan/Utilities/LogMacro.h"

void UEnableCollisionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration)
{
	//if (MeshComp && MeshComp->GetOwner())
	//{
	//	HitActors.Empty();
	//
	//	m_Owner = Cast<AAreaObject>(MeshComp->GetOwner());
	//	if (m_Owner != nullptr)
	//	{
	//		AttackData = m_Owner->GetCurrentSkillAttackData(AttackDataIndex);
	//		if (AttackData == nullptr) LOG_PRINT(TEXT("AttackData is Empty"));
	//
	//		// Test용 디버그 데이터 로직
	//		if (bDebugData == true)
	//		{
	//			LOG_SCREEN("디버그용 데이터로 계산!! AnimNotifyState 설정 변경하기!")
	//			AttackData = &DebugAttackData;
	//		}
	//		
	//		OwnerSourceMesh = GetTargetMesh(m_Owner);
	//	}
	//}
	if (MeshComp && MeshComp->GetOwner())
	{
		HitActors.Empty();

		m_Owner = Cast<AAreaObject>(MeshComp->GetOwner());
		if (m_Owner != nullptr && m_Owner->GetCurrentSkill() != nullptr)
		{
			UCollisionSkill* collisionSkill = Cast<UCollisionSkill>(m_Owner->GetCurrentSkill());
			if (collisionSkill != nullptr)
			{
				collisionSkill->SetCasterMesh(AttackDataIndex);
			}
		}
	}
}

void UEnableCollisionNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float FrameDeltaTime)
{
	//if (OwnerSourceMesh && m_Owner && AttackData)
	//{
	//	ProcessHitDetection(m_Owner, OwnerSourceMesh);
	//}
}

void UEnableCollisionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		HitActors.Empty();

		m_Owner = Cast<AAreaObject>(MeshComp->GetOwner());
		if (m_Owner != nullptr && m_Owner->GetCurrentSkill() != nullptr)
		{
			UCollisionSkill* collisionSkill = Cast<UCollisionSkill>(m_Owner->GetCurrentSkill());
			if (collisionSkill != nullptr)
			{
				collisionSkill->ResetCollisionData(AttackDataIndex);
			}
		}
	}
	//HitActors.Empty();
}

USkeletalMeshComponent* UEnableCollisionNotifyState::GetTargetMesh(AAreaObject* TargetAreaObject) const
{
	if (AttackData == nullptr)
	{
		LOG_PRINT(TEXT("AttackData is Empty"));
		return nullptr;
	}
	if (AttackData->HitBoxData.MeshComponentTag == NAME_None)
	{
		return TargetAreaObject->GetMesh();
	}
	// 태그로 지정된 메시 찾기
	TArray<UActorComponent*> Components;
	TargetAreaObject->GetComponents(USkeletalMeshComponent::StaticClass(), Components);

	for (UActorComponent* Component : Components)
	{
		if (Component->ComponentHasTag(AttackData->HitBoxData.MeshComponentTag))
		{
			return Cast<USkeletalMeshComponent>(Component);
		}
	}
	return nullptr;
}

void UEnableCollisionNotifyState::ProcessHitDetection(AAreaObject* OwnerAreaObject, USkeletalMeshComponent* SourceMesh)
{
	if (!SourceMesh || !OwnerAreaObject)
		return;

	FVector StartLocation = SourceMesh->GetSocketLocation(AttackData->HitBoxData.StartSocketName);
	FVector EndLocation = AttackData->HitBoxData.EndSocketName != NAME_None
		                      ? SourceMesh->GetSocketLocation(AttackData->HitBoxData.EndSocketName)
		                      : StartLocation;
	FRotator SocketRotation = SourceMesh->GetSocketRotation(AttackData->HitBoxData.StartSocketName);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerAreaObject);

	TArray<FHitResult> HitResults;
	bool bHit = false;


	switch (AttackData->HitBoxData.DetectionType)
	{
	case EHitDetectionType::Line:
		{
			bHit = m_Owner->GetWorld()->LineTraceMultiByChannel(
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
			bHit = m_Owner->GetWorld()->SweepMultiByChannel(
				HitResults,
				StartLocation,
				EndLocation,
				FQuat::Identity,
				ECC_GameTraceChannel2,
				FCollisionShape::MakeSphere(AttackData->HitBoxData.Radius),
				QueryParams
			);
			break;
		}

	case EHitDetectionType::Capsule:
		{
			FVector location = (StartLocation + EndLocation) / 2.f;
			bHit = m_Owner->GetWorld()->SweepMultiByChannel(
				HitResults,
				location,
				location,
				SocketRotation.Quaternion(),
				ECC_GameTraceChannel2,
				FCollisionShape::MakeCapsule(AttackData->HitBoxData.Radius,
				                             AttackData->HitBoxData.HalfHeight),
				QueryParams
			);
			break;
		}

	case EHitDetectionType::Box:
		{
			FVector location = (StartLocation + EndLocation) / 2.f;
			bHit = m_Owner->GetWorld()->SweepMultiByChannel(
				HitResults,
				location,
				location,
				SocketRotation.Quaternion(),
				ECC_GameTraceChannel2,
				FCollisionShape::MakeBox(AttackData->HitBoxData.BoxExtent),
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
			m_Owner->CalcDamage(*AttackData, m_Owner, hitActor, Hit);
		}
	}
}

void UEnableCollisionNotifyState::DrawDebugHitDetection(const FVector& Start, const FVector& End,
                                                        const TArray<FHitResult>& HitResults,
                                                        const FRotator& SocketRotation) const
{
	auto& HitBoxData = AttackData->HitBoxData;
	UWorld* World = m_Owner->GetWorld();
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
