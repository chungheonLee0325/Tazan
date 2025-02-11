﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "EnableCollisionNotifyState.h"

#include "Tazan/AreaObject/Base/AreaObject.h"

void UEnableCollisionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		HitActors.Empty();

		m_Owner = Cast<AAreaObject>(MeshComp->GetOwner());
		if (m_Owner != nullptr)
		{
			OwnerSourceMesh = GetTargetMesh(m_Owner);
		}
	}
}

void UEnableCollisionNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float FrameDeltaTime)
{
	if (OwnerSourceMesh && m_Owner)
	{
		ProcessHitDetection(m_Owner, OwnerSourceMesh);
	}
}

void UEnableCollisionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	HitActors.Empty();
}

USkeletalMeshComponent* UEnableCollisionNotifyState::GetTargetMesh(AAreaObject* TargetAreaObject) const
{
	if (SkillData.AttackData.HitBoxData.MeshComponentTag == NAME_None)
	{
		return TargetAreaObject->GetMesh();
	}
	// 태그로 지정된 메시 찾기
	TArray<UActorComponent*> Components;
	TargetAreaObject->GetComponents(USkeletalMeshComponent::StaticClass(), Components);

	for (UActorComponent* Component : Components)
	{
		if (Component->ComponentHasTag(SkillData.AttackData.HitBoxData.MeshComponentTag))
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

	FVector StartLocation = SourceMesh->GetSocketLocation(SkillData.AttackData.HitBoxData.StartSocketName);
	//FVector EndLocation = SourceMesh->GetSocketLocation(SkillData.AttackData.HitBoxData.EndSocketName);
	FVector EndLocation = SkillData.AttackData.HitBoxData.EndSocketName != NAME_None
		                      ? SourceMesh->GetSocketLocation(SkillData.AttackData.HitBoxData.EndSocketName)
		                      : StartLocation;
	FRotator SocketRotation = SourceMesh->GetSocketRotation(SkillData.AttackData.HitBoxData.StartSocketName);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerAreaObject);

	TArray<FHitResult> HitResults;
	bool bHit = false;

	switch (SkillData.AttackData.HitBoxData.DetectionType)
	{
	case EHitDetectionType::Line:
		{
			bHit = GetWorld()->LineTraceMultiByChannel(
				HitResults,
				StartLocation,
				EndLocation,
				ECC_Pawn,
				QueryParams
			);
			break;
		}

	case EHitDetectionType::Sphere:
		{
			bHit = GetWorld()->SweepMultiByChannel(
				HitResults,
				StartLocation,
				EndLocation,
				FQuat::Identity,
				ECC_Pawn,
				FCollisionShape::MakeSphere(SkillData.AttackData.HitBoxData.Radius),
				QueryParams
			);
			break;
		}

	case EHitDetectionType::Capsule:
		{
			bHit = GetWorld()->SweepMultiByChannel(
				HitResults,
				StartLocation,
				EndLocation,
				SocketRotation.Quaternion(),
				ECC_Pawn,
				FCollisionShape::MakeCapsule(SkillData.AttackData.HitBoxData.Radius,
				                             SkillData.AttackData.HitBoxData.HalfHeight),
				QueryParams
			);
			break;
		}

	case EHitDetectionType::Box:
		{
			bHit = GetWorld()->SweepMultiByChannel(
				HitResults,
				StartLocation,
				EndLocation,
				SocketRotation.Quaternion(),
				ECC_Pawn,
				FCollisionShape::MakeBox(SkillData.AttackData.HitBoxData.BoxExtent),
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
	for (const FHitResult& Hit : HitResults)
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
			// Calc Point Damage
		}
	}
}

void UEnableCollisionNotifyState::DrawDebugHitDetection(const FVector& Start, const FVector& End,
                                                        const TArray<FHitResult>& HitResults,
                                                        const FRotator& SocketRotation) const
{
	auto& HitBoxData = SkillData.AttackData.HitBoxData;
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
