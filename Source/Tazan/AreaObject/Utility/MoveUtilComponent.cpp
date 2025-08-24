#include "MoveUtilComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
#include "Tazan/AreaObject/Player/Component/LockOnComponent.h"

static float ToYawDeg(ERelMoveDir D, float Custom)
{
	switch (D)
	{
	case ERelMoveDir::Forward: return 0.f;
	case ERelMoveDir::Backward: return 180.f;
	case ERelMoveDir::Left: return -90.f;
	case ERelMoveDir::Right: return 90.f;
	case ERelMoveDir::Target: return 0.f; // Target은 별도 처리
	case ERelMoveDir::CustomYaw: default: return Custom;
	}
}


AActor* UMoveUtilComponent::ResolveAreaTarget(AAreaObject* Area, bool bUseLockOn)
{
	if (!Area) return nullptr;

	// 1) 플레이어: 락온 우선
	if (bUseLockOn)
	{
		if (auto* Player = Cast<APlayer_Kazan>(Area))
		{
			if (auto* L = Player->GetLockOnComponent())
			{
				if (L->IsLockOnMode())
				{
					if (AActor* T = L->GetCurrentTarget()) return T;
				}
			}
		}
	}

	// 2) 몬스터: 어그로 타깃
	if (auto* Monster = Cast<ABaseMonster>(Area))
	{
		if (AActor* Aggro = Monster->GetAggroTarget()) return Aggro;
	}

	return nullptr;
}

void UMoveUtilComponent::AutoResolveTargetsIfNeeded(FAreaMoveSpec& S, AAreaObject* AreaObject,
                                                    bool bUseLockOnToward, bool bUseLockOnFacing)
{
	if (!AreaObject) return;

	if (S.Intent == EMoveIntent::TowardActor && !S.TowardActor.IsValid())
	{
		S.TowardActor = ResolveAreaTarget(AreaObject, bUseLockOnToward);
	}
	if (S.Intent == EMoveIntent::InFacing && S.FacingDir == ERelMoveDir::Target && !S.TargetActor.IsValid())
	{
		S.TargetActor = ResolveAreaTarget(AreaObject, bUseLockOnFacing);
	}
}

FVector UMoveUtilComponent::ComputeTowardTargetPos(AActor* Owner, AActor* Target, float StopDistance)
{
	if (!Owner || !Target) return Owner ? Owner->GetActorLocation() : FVector::ZeroVector;

	FVector OwnerLoc = Owner->GetActorLocation();
	FVector TargetLoc = Target->GetActorLocation();

	FVector Dir = TargetLoc - OwnerLoc;
	Dir.Z = 0.f;
	const float Dist2D = Dir.Size2D();
	Dir = (Dist2D > KINDA_SMALL_NUMBER) ? Dir / Dist2D : FVector::ForwardVector;

	float ExtraRadius = 0.f;
	if (const ACharacter* C = Cast<ACharacter>(Owner))
		if (const UCapsuleComponent* Cap = C->GetCapsuleComponent())
			ExtraRadius += Cap->GetScaledCapsuleRadius();
	if (const ACharacter* CT = Cast<ACharacter>(Target))
		if (const UCapsuleComponent* CapT = CT->GetCapsuleComponent())
			ExtraRadius += CapT->GetScaledCapsuleRadius();

	const float Stop = FMath::Max(0.f, StopDistance) + ExtraRadius;
	const float Travel = FMath::Max(0.f, Dist2D - Stop);

	return OwnerLoc + Dir * Travel;
}


UMoveUtilComponent::UMoveUtilComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMoveUtilComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMoveUtilComponent::StartMove(const FAreaMoveSpec& Spec)
{
	// 우선순위 비교
	if (MovementState.bIsActive && Spec.Priority < MovementState.Priority)
	{
		// 낮은 우선순위 → 무시
		return;
	}

	// 기존 이동 교체
	if (MovementState.bIsActive)
	{
		StopMovement(EMoveFinishReason::Replaced);
	}

	BeginNewMovement(Spec);
}

void UMoveUtilComponent::BeginNewMovement(const FAreaMoveSpec& Spec)
{
	MovementState.Reset();
	MovementState.bIsActive = true;
	MovementState.Spec = Spec;
	MovementState.SourceId = Spec.SourceId;
	MovementState.Priority = Spec.Priority;
	MovementState.bSlideOnBlock = Spec.bSlideOnBlock;

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		StopMovement(EMoveFinishReason::Canceled);
		return;
	}

	MovementState.StartLoc = Owner->GetActorLocation();

	// 목표 계산
	switch (Spec.Intent)
	{
	case EMoveIntent::ToLocation:
		MovementState.TargetLoc = Spec.TargetLocation;
		break;

	case EMoveIntent::InFacing:
		ComputeFacingTarget(Spec, MovementState.TargetLoc);
		break;

	case EMoveIntent::TowardActor:
		ComputeTowardTarget(Spec, MovementState.TargetLoc);
		break;
	}

	// 총 거리(2D)
	MovementState.TotalDistance = FVector::Dist2D(MovementState.StartLoc, MovementState.TargetLoc);
	MovementState.TotalDistance = FMath::Max(1.f, MovementState.TotalDistance); // 0 방지

	// 표현 통일: Duration 계산
	if (Spec.Kinematics == EKinematics::ByDuration)
	{
		MovementState.Duration = FMath::Max(KINDA_SMALL_NUMBER, Spec.Duration);
	}
	else
	{
		const float Speed = FMath::Max(KINDA_SMALL_NUMBER, Spec.Speed);
		MovementState.Duration = MovementState.TotalDistance / Speed;
	}

	MovementState.CurrentTime = 0.f;
	MovementState.AccumulatedDelta = 0.f;

	StartTimeoutTimerIfNeeded(Spec);
}

void UMoveUtilComponent::UpdateMove(const FAreaMoveUpdate& Update)
{
	if (!MovementState.bIsActive) return;
	if (Update.SourceId != MovementState.SourceId) return;

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		StopMovement(EMoveFinishReason::Canceled);
		return;
	}

	// 현재 위치를 새로운 시작점으로
	MovementState.StartLoc = Owner->GetActorLocation();

	if (Update.bRetarget)
	{
		MovementState.TargetLoc = Update.NewTargetLocation;
	}

	// Duration 재산정
	if (Update.bChangeKinetics)
	{
		MovementState.Spec.Kinematics = Update.NewKinematics;
		if (Update.NewKinematics == EKinematics::ByDuration)
		{
			MovementState.Duration = FMath::Max(KINDA_SMALL_NUMBER, Update.NewSpeedOrDuration);
		}
		else
		{
			const float Speed = FMath::Max(KINDA_SMALL_NUMBER, Update.NewSpeedOrDuration);
			MovementState.TotalDistance = FVector::Dist2D(MovementState.StartLoc, MovementState.TargetLoc);
			MovementState.TotalDistance = FMath::Max(1.f, MovementState.TotalDistance);
			MovementState.Duration = MovementState.TotalDistance / Speed;
		}
	}
	else
	{
		if (MovementState.Spec.Kinematics == EKinematics::BySpeed)
		{
			const float Speed = FMath::Max(KINDA_SMALL_NUMBER, MovementState.Spec.Speed);
			MovementState.TotalDistance = FVector::Dist2D(MovementState.StartLoc, MovementState.TargetLoc);
			MovementState.TotalDistance = FMath::Max(1.f, MovementState.TotalDistance);
			MovementState.Duration = MovementState.TotalDistance / Speed;
		}
		// ByDuration은 Duration 유지
	}

	MovementState.CurrentTime = 0.f;
	MovementState.AccumulatedDelta = 0.f;
}

void UMoveUtilComponent::StopMoveBySourceId(int32 SourceId, EMoveFinishReason Reason)
{
	if (MovementState.bIsActive && MovementState.SourceId == SourceId)
	{
		StopMovement(Reason);
	}
}

void UMoveUtilComponent::StopAllMoves(EMoveFinishReason Reason)
{
	if (MovementState.bIsActive)
	{
		StopMovement(Reason);
	}
}

void UMoveUtilComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!MovementState.bIsActive) return;
	if (DeltaTime <= 0.f) return;

	// 누적 스텝 업데이트
	MovementState.AccumulatedDelta += DeltaTime;
	const bool bUseStep = (MinUpdateInterval > 0.f);
	const float Step = bUseStep
		                   ? (MovementState.AccumulatedDelta >= MinUpdateInterval
			                      ? MovementState.AccumulatedDelta
			                      : 0.f)
		                   : DeltaTime;

	if (Step <= 0.f) return;
	MovementState.AccumulatedDelta = 0.f;

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		StopMovement(EMoveFinishReason::Canceled);
		return;
	}

	MovementState.CurrentTime += Step;

	const float RawAlpha = FMath::Clamp(MovementState.CurrentTime / MovementState.Duration, 0.f, 1.f);
	const float Alpha = CalcInterpAlpha(RawAlpha, MovementState.Spec.Interp, MovementState.Spec.Curve);

	const FVector NewPosUnstuck = FMath::Lerp(MovementState.StartLoc, MovementState.TargetLoc, Alpha);
	FVector NewPos = NewPosUnstuck;

	if (MovementState.Spec.bStickToGround)
	{
		ApplyGroundStick(NewPos);
	}

	const FVector CurrentLoc = Owner->GetActorLocation();
	const FVector Delta = (NewPos - CurrentLoc);

	FHitResult Hit;
	const bool bMoved = TryMoveOwner(Delta, Hit);
	const bool bBlocked = (!bMoved) || Hit.bBlockingHit;

	if (bBlocked && !MovementState.bSlideOnBlock)
	{
		StopMovement(EMoveFinishReason::Blocked);
		return;
	}

	if (RawAlpha >= 1.f)
	{
		StopMovement(EMoveFinishReason::Reached);
		return;
	}

	if (bShowDebugPath) DrawDebug();
}

void UMoveUtilComponent::StopMovement(EMoveFinishReason Reason)
{
	ClearTimeoutTimer();

	const int32 Source = MovementState.SourceId;
	MovementState.Reset();

	OnMoveFinished.Broadcast(Reason, Source);
}

// ---------- 타깃 계산 ----------

void UMoveUtilComponent::ComputeFacingTarget(const FAreaMoveSpec& Spec, FVector& OutTarget) const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		OutTarget = FVector::ZeroVector;
		return;
	}

	// 기본 방향
	FVector Dir;
	if (Spec.FacingDir == ERelMoveDir::Target && Spec.TargetActor.IsValid())
	{
		// 타깃 방향 (2D)
		Dir = Spec.TargetActor->GetActorLocation() - Owner->GetActorLocation();
		Dir.Z = 0.f;
		if (Dir.SizeSquared2D() > KINDA_SMALL_NUMBER)
		{
			Dir.Normalize();
			// 추가 요 오프셋이 있다면 적용
			if (!FMath::IsNearlyZero(Spec.YawOffsetDeg))
			{
				Dir = FRotationMatrix(FRotator(0.f, Spec.YawOffsetDeg, 0.f)).TransformVector(Dir);
			}
		}
		else
		{
			Dir = Owner->GetActorForwardVector().GetSafeNormal2D();
		}
	}
	else
	{
		const FVector Fwd = Owner->GetActorForwardVector();
		const float Yaw = ToYawDeg(Spec.FacingDir, Spec.YawOffsetDeg);
		Dir = FRotationMatrix(FRotator(0.f, Yaw, 0.f)).TransformVector(Fwd).GetSafeNormal();
	}

	float Distance = Spec.Distance;
	if (Spec.Kinematics == EKinematics::ByDuration)
	{
		Distance = FMath::Max(0.f, Spec.Speed) * FMath::Max(0.f, Spec.Duration);
	}

	OutTarget = Owner->GetActorLocation() + Dir * Distance;
}

float UMoveUtilComponent::GetCombinedCapsuleRadius2D(const AActor* A, const AActor* B) const
{
	float R = 0.f;
	if (const ACharacter* CA = Cast<ACharacter>(A))
		if (const UCapsuleComponent* Cap = CA->GetCapsuleComponent())
			R += Cap->GetScaledCapsuleRadius();

	if (const ACharacter* CB = Cast<ACharacter>(B))
		if (const UCapsuleComponent* Cap = CB->GetCapsuleComponent())
			R += Cap->GetScaledCapsuleRadius();

	return R;
}

void UMoveUtilComponent::ComputeTowardTarget(const FAreaMoveSpec& Spec, FVector& OutTarget) const
{
	AActor* Owner = GetOwner();
	AActor* Target = Spec.TowardActor.Get();
	if (!Owner)
	{
		OutTarget = FVector::ZeroVector;
		return;
	}
	if (!Target)
	{
		OutTarget = Owner->GetActorLocation();
		return;
	}

	const FVector OwnerLoc = Owner->GetActorLocation();
	const FVector TargetLoc = Target->GetActorLocation();

	FVector Dir = TargetLoc - OwnerLoc;
	Dir.Z = 0.f;
	const float Dist2D = Dir.Size2D();
	Dir = (Dist2D > KINDA_SMALL_NUMBER) ? Dir / Dist2D : FVector::ForwardVector;

	if (Spec.TowardPolicy == ETowardPolicy::FixedTravel)
	{
		// 고정 이동형: 항상 일정 거리(또는 속도*시간)
		float Travel = 0.f;
		if (Spec.Kinematics == EKinematics::ByDuration)
		{
			Travel = FMath::Max(0.f, Spec.Speed) * FMath::Max(0.f, Spec.Duration);
		}
		// BySpeed
		else
		{
			Travel = FMath::Max(0.f, Spec.TravelDistance);
		}
		OutTarget = OwnerLoc + Dir * Travel;
		return;
	}

	// ReachStopDistance (기존)
	const float Stop = FMath::Max(0.f, Spec.StopDistance) + GetCombinedCapsuleRadius2D(Owner, Target);
	const float Travel = FMath::Max(0.f, Dist2D - Stop);
	OutTarget = OwnerLoc + Dir * Travel;
}

// ---------- 보간/지면/이동 ----------

float UMoveUtilComponent::CalcInterpAlpha(float RawAlpha, EMovementInterpolationType InterpType,
                                          const UCurveFloat* Curve) const
{
	if (Curve) return Curve->GetFloatValue(RawAlpha);

	switch (InterpType)
	{
	case EMovementInterpolationType::Linear: return RawAlpha;
	case EMovementInterpolationType::EaseIn: return RawAlpha * RawAlpha;
	case EMovementInterpolationType::EaseOut: return 1.f - FMath::Square(1.f - RawAlpha);
	case EMovementInterpolationType::EaseInOut: return 0.5f - 0.5f * FMath::Cos(PI * RawAlpha);
	default: return RawAlpha;
	}
}

void UMoveUtilComponent::ApplyGroundStick(FVector& InOutPosition) const
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	const FVector Start = InOutPosition + FVector(0, 0, GroundTraceUp);
	const FVector End = InOutPosition - FVector(0, 0, GroundTraceDown);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MoveUtil_GroundStick), false, Owner);
	Params.AddIgnoredActor(Owner);

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	if (Hit.bBlockingHit)
	{
		InOutPosition.Z = Hit.Location.Z;
	}
}

bool UMoveUtilComponent::TryMoveOwner(const FVector& Delta, FHitResult& OutHit) const
{
	AActor* Owner = GetOwner();
	if (!Owner) return false;

	if (ACharacter* C = Cast<ACharacter>(Owner))
	{
		if (UCharacterMovementComponent* CM = C->GetCharacterMovement())
		{
			CM->SafeMoveUpdatedComponent(Delta, C->GetActorQuat(), true, OutHit);
			return !OutHit.bBlockingHit; // 이동 성공 여부
		}
	}

	const bool bMoved = Owner->SetActorLocation(Owner->GetActorLocation() + Delta, true, &OutHit, ETeleportType::None);
	return bMoved; // blocking이면 false
}

// ---------- 타임아웃/디버그 ----------

void UMoveUtilComponent::StartTimeoutTimerIfNeeded(const FAreaMoveSpec& Spec)
{
	ClearTimeoutTimer();

	if (Spec.MaxDuration <= 0.f) return;
	if (!GetWorld()) return;

	GetWorld()->GetTimerManager().SetTimer(
		MovementState.TimeoutHandle,
		FTimerDelegate::CreateUObject(this, &UMoveUtilComponent::StopMovement, EMoveFinishReason::Timeout),
		Spec.MaxDuration,
		false
	);
}

void UMoveUtilComponent::ClearTimeoutTimer()
{
	if (GetWorld() && MovementState.TimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MovementState.TimeoutHandle);
	}
}

void UMoveUtilComponent::DrawDebug() const
{
	if (!bShowDebugPath || !GetOwner()) return;

	const FVector A = MovementState.StartLoc;
	const FVector B = MovementState.TargetLoc;

	DrawDebugSphere(GetWorld(), A, 8.f, 8, FColor::Green, false, 0.05f);
	DrawDebugSphere(GetWorld(), B, 8.f, 8, FColor::Red, false, 0.05f);
	DrawDebugLine(GetWorld(), A, B, FColor::Cyan, false, 0.05f, 0, 2.f);
}
