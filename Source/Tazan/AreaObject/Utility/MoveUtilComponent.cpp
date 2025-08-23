#include "MoveUtilComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"

UMoveUtilComponent::UMoveUtilComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UMoveUtilComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UMoveUtilComponent::MoveActorTo(const FVector& Target, float Duration,
                                     EMovementInterpolationType Interp,
                                     bool bStickToGround,
                                     int32 Priority, int32 SourceId,
                                     bool bSlideOnBlock,
                                     UCurveFloat* Curve)
{
    StartNewMovement(Target, EPMMovementMode::Duration, Duration, Interp, bStickToGround, Priority, SourceId, bSlideOnBlock, Curve);
}

void UMoveUtilComponent::MoveActorToWithSpeed(const FVector& Target, float Speed,
                                              EMovementInterpolationType Interp,
                                              bool bStickToGround,
                                              int32 Priority, int32 SourceId,
                                              bool bSlideOnBlock,
                                              UCurveFloat* Curve)
{
    StartNewMovement(Target, EPMMovementMode::Speed, Speed, Interp, bStickToGround, Priority, SourceId, bSlideOnBlock, Curve);
}

void UMoveUtilComponent::MoveInFacing(float Distance, float Speed,
                                      EMovementInterpolationType Interp,
                                      bool bStickToGround,
                                      int32 Priority, int32 SourceId,
                                      bool bSlideOnBlock,
                                      UCurveFloat* Curve)
{
    if (!GetOwner()) return;
    const FVector Forward = GetOwner()->GetActorForwardVector();
    const FVector Target = GetOwner()->GetActorLocation() + Forward * Distance;
    MoveActorToWithSpeed(Target, Speed, Interp, bStickToGround, Priority, SourceId, bSlideOnBlock, Curve);
}

void UMoveUtilComponent::MoveTowardsActor(AActor* TargetActor, float StopDistance, float Speed,
                                          EMovementInterpolationType Interp,
                                          bool bStickToGround,
                                          int32 Priority, int32 SourceId,
                                          bool bSlideOnBlock,
                                          UCurveFloat* Curve)
{
    if (!GetOwner() || !TargetActor) return;

    FVector OwnerLoc = GetOwner()->GetActorLocation();
    FVector TargetLoc = TargetActor->GetActorLocation();

    // 2D 방향으로 계산(지면 전제)
    FVector Dir = (TargetLoc - OwnerLoc);
    Dir.Z = 0.f;
    Dir = Dir.GetSafeNormal();

    float ExtraRadius = 0.f;
    if (const ACharacter* Ch = Cast<ACharacter>(GetOwner()))
    {
        if (const UCapsuleComponent* Cap = Ch->GetCapsuleComponent())
            ExtraRadius += Cap->GetScaledCapsuleRadius();
    }
    if (const ACharacter* ChT = Cast<ACharacter>(TargetActor))
    {
        if (const UCapsuleComponent* CapT = ChT->GetCapsuleComponent())
            ExtraRadius += CapT->GetScaledCapsuleRadius();
    }

    const float Stop = FMath::Max(StopDistance, 0.f) + ExtraRadius;
    const float Dist = FVector::Dist2D(TargetLoc, OwnerLoc);
    const float Travel = FMath::Max(Dist - Stop, 0.f);

    const FVector FinalTarget = OwnerLoc + Dir * Travel;
    MoveActorToWithSpeed(FinalTarget, Speed, Interp, bStickToGround, Priority, SourceId, bSlideOnBlock, Curve);
}

bool UMoveUtilComponent::UpdateMovementTarget(const FVector& NewTarget,
                                              bool bChangeSpeed, float NewSpeedOrDuration,
                                              bool bRequireSameSourceId,
                                              int32 SourceId)
{
    if (!MovementState.IsActive()) return false;
    if (bRequireSameSourceId && MovementState.SourceId != SourceId) return false;

    MovementState.TargetPos = NewTarget;

    if (bChangeSpeed)
    {
        if (MovementState.MovementMode == EPMMovementMode::Speed)
        {
            MovementState.Speed = NewSpeedOrDuration;
            const float Dist = (MovementState.TargetPos - MovementState.StartPos).Size();
            MovementState.Duration = (MovementState.Speed > KINDA_SMALL_NUMBER) ? Dist / MovementState.Speed : 0.f;
        }
        else
        {
            MovementState.Duration = NewSpeedOrDuration;
        }
        MovementState.CurrentTime = FMath::Min(MovementState.CurrentTime, MovementState.Duration);
    }
    return true;
}

void UMoveUtilComponent::CancelBySourceId(int32 InSourceId, EMoveFinishReason Reason)
{
    if (MovementState.IsActive() && MovementState.SourceId == InSourceId)
    {
        ClearCancelTimer(InSourceId); 
        StopMovement(Reason);
    }
}

void UMoveUtilComponent::StopMovement(EMoveFinishReason Reason)
{
    if (!MovementState.IsActive()) return;
    const int32 FinishedSource = MovementState.SourceId;
    MovementState.Stop();
    ClearCancelTimer(FinishedSource);
    OnMoveFinished.Broadcast(Reason, FinishedSource);
}

void UMoveUtilComponent::StartNewMovement(const FVector& Target, EPMMovementMode Mode, float SpeedOrDuration,
                                          EMovementInterpolationType Interp, bool bStickToGround,
                                          int32 Priority, int32 SourceId, bool bSlideOnBlock, UCurveFloat* Curve)
{
    // Priority: ignore if lower than current
    if (MovementState.IsActive() && Priority < MovementState.Priority)
    {
        return;
    }

    // Replace current
    if (MovementState.IsActive())
    {
        StopMovement(EMoveFinishReason::Replaced);
    }

    MovementState = FMovementState{};
    MovementState.StartPos = GetOwner()->GetActorLocation();
    MovementState.TargetPos = Target;
    MovementState.MovementMode = Mode;
    MovementState.InterpType = Interp;
    MovementState.bStickToGround = bStickToGround;
    MovementState.bSlideOnBlock = bSlideOnBlock;
    MovementState.Priority = Priority;
    MovementState.SourceId = SourceId;
    MovementState.Curve = Curve;

    if (Mode == EPMMovementMode::Duration)
    {
        MovementState.Duration = SpeedOrDuration;
        MovementState.Speed = 0.f;
    }
    else
    {
        MovementState.Speed = SpeedOrDuration;
        const float Dist = (Target - MovementState.StartPos).Size();
        MovementState.Duration = (SpeedOrDuration > KINDA_SMALL_NUMBER) ? Dist / SpeedOrDuration : 0.f;
    }

    MovementState.CurrentTime = 0.f;
    MovementState.bIsActive = true;
}

static float EaseElastic(float A)
{
    // simple elastic
    return FMath::Sin(-13.f * (PI/2.f) * (A + 1.f)) * FMath::Pow(2.f, -10.f * A) + 1.f;
}

float UMoveUtilComponent::CalculateInterpolationAlpha(float RawAlpha, EMovementInterpolationType InterpType, const UCurveFloat* Curve)
{
    float A = FMath::Clamp(RawAlpha, 0.f, 1.f);
    if (Curve) return Curve->GetFloatValue(A);

    switch (InterpType)
    {
        case EMovementInterpolationType::Linear:      return A;
        case EMovementInterpolationType::EaseIn:      return FMath::InterpEaseIn(0.f, 1.f, A, 2.f);
        case EMovementInterpolationType::EaseOut:     return FMath::InterpEaseOut(0.f, 1.f, A, 2.f);
        case EMovementInterpolationType::EaseInOut:   return FMath::InterpEaseInOut(0.f, 1.f, A, 2.f);
        case EMovementInterpolationType::ExpoIn:      return FMath::Pow(2.f, 10.f * (A - 1.f));
        case EMovementInterpolationType::ExpoOut:     return (A >= 1.f) ? 1.f : (1.f - FMath::Pow(2.f, -10.f * A));
        case EMovementInterpolationType::ExpoInOut:   return (A < 0.5f) ? 0.5f * FMath::Pow(2.f, (20.f * A) - 10.f)
                                                                      : 1.f - 0.5f * FMath::Pow(2.f, (-20.f * A) + 10.f);
        case EMovementInterpolationType::ElasticIn:   return 1.f - EaseElastic(1.f - A);
        case EMovementInterpolationType::ElasticOut:  return EaseElastic(A);
        case EMovementInterpolationType::ElasticInOut: return (A < 0.5f) ? 0.5f * (1.f - EaseElastic(1.f - 2.f*A))
                                                                        : 0.5f * (EaseElastic(2.f*A - 1.f) + 1.f);
        default: return A;
    }
}

void UMoveUtilComponent::ApplyGroundStick(FVector& P) const
{
    if (!GetOwner()) return;

    const FVector Start = P + FVector(0, 0, MovementState.TraceUp);
    const FVector End   = P - FVector(0, 0, MovementState.TraceDown);

    FHitResult Hit;
    FCollisionQueryParams QParams(SCENE_QUERY_STAT(MoveUtil_GroundStick), false);
    QParams.AddIgnoredActor(GetOwner());

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, QParams))
    {
        P.Z = Hit.Location.Z;
    }
}

bool UMoveUtilComponent::SafeMoveOwner(const FVector& Delta, FHitResult& OutHit)
{
    if (!GetOwner()) return false;

    if (ACharacter* Ch = Cast<ACharacter>(GetOwner()))
    {
        if (UCharacterMovementComponent* CMC = Ch->GetCharacterMovement())
        {
            CMC->SafeMoveUpdatedComponent(Delta, Ch->GetActorRotation(), true, OutHit);
            const bool bBlocked = OutHit.bBlockingHit;

            if (bBlocked && !MovementState.bSlideOnBlock)
            {
                // 슬라이드 금지 모드: 즉시 차단 끝내기
                return false; // 호출부에서 StopMovement(Blocked)
            }
            return true;
        }
    }

    const FVector NewPos = GetOwner()->GetActorLocation() + Delta;
    return GetOwner()->SetActorLocation(NewPos, true, &OutHit, ETeleportType::None);
}

void UMoveUtilComponent::DrawDebugVisuals() const
{
    if (!bShowDebugPath || !GetWorld() || !MovementState.IsActive()) return;

    DrawDebugLine(GetWorld(), MovementState.StartPos, MovementState.TargetPos, FColor::Cyan, false, 0.f, 0, 1.f);
    DrawDebugSphere(GetWorld(), MovementState.TargetPos, 8.f, 12, FColor::Green, false, 0.f);
}

void UMoveUtilComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!MovementState.IsActive())
    {
        if (bShowDebugPath) DrawDebugVisuals();
        return;
    }

    // Perf budget
    if (MinUpdateInterval > 0.f)
    {
        LastUpdateTime += DeltaTime;
        if (LastUpdateTime < MinUpdateInterval) return;
        DeltaTime = LastUpdateTime; // accumulate
        LastUpdateTime = 0.f;
    }

    MovementState.CurrentTime += DeltaTime;
    const float Alpha = MovementState.GetProgress();

    const float InterpAlpha = CalculateInterpolationAlpha(Alpha, MovementState.InterpType, MovementState.Curve);
    FVector Desired = FMath::Lerp(MovementState.StartPos, MovementState.TargetPos, InterpAlpha);

    // Continuous ground follow
    if (MovementState.bStickToGround)
    {
        ApplyGroundStick(Desired);
    }

    AActor* Owner = GetOwner();
    if (!Owner) { StopMovement(EMoveFinishReason::Canceled); return; }

    FVector Delta = Desired - Owner->GetActorLocation();
    if (Delta.SizeSquared() < FMath::Square(LocationUpdateThreshold))
    {
        if (Alpha >= 1.f) { StopMovement(EMoveFinishReason::Reached); }
        return;
    }

    FHitResult Hit;
    const bool bMoved = SafeMoveOwner(Delta, Hit);
    const bool bBlocked = (!bMoved) || Hit.bBlockingHit;

    if (bBlocked && !MovementState.bSlideOnBlock)
    {
        StopMovement(EMoveFinishReason::Blocked);
        return;
    }

    if (Alpha >= 1.f)
    {
        StopMovement(EMoveFinishReason::Reached);
    }

    if (bShowDebugPath) DrawDebugVisuals();
}

void UMoveUtilComponent::MoveInFacingTimed(float Speed, float Duration,
                                           EMovementInterpolationType Interp,
                                           bool bStickToGround,
                                           int32 Priority, int32 SourceId,
                                           bool bSlideOnBlock,
                                           UCurveFloat* Curve)
{
    if (!GetOwner()) return;
    const FVector Start = GetOwner()->GetActorLocation();
    const FVector Fwd   = GetOwner()->GetActorForwardVector();
    const FVector Target = Start + (Fwd * FMath::Max(Speed, 0.f) * FMath::Max(Duration, 0.f));
    // Duration 모드로 고정 시간 보간
    StartNewMovement(Target, EPMMovementMode::Duration, Duration, Interp, bStickToGround, Priority, SourceId, bSlideOnBlock, Curve);
}

void UMoveUtilComponent::MoveTowardsActorTimed(AActor* TargetActor, float StopDistance, float Speed, float MaxDuration,
                                               EMovementInterpolationType Interp,
                                               bool bStickToGround,
                                               int32 Priority, int32 SourceId,
                                               bool bSlideOnBlock,
                                               UCurveFloat* Curve)
{
    // 기존 로직 재사용
    MoveTowardsActor(TargetActor, StopDistance, Speed, Interp, bStickToGround, Priority, SourceId, bSlideOnBlock, Curve);

    if (MaxDuration > 0.f)
    {
        StartCancelTimer(MaxDuration, SourceId, EMoveFinishReason::Timeout);
    }
}

void UMoveUtilComponent::StartCancelTimer(float Seconds, int32 SourceId, EMoveFinishReason Reason)
{
    if (!GetWorld()) return;
    ClearCancelTimer(SourceId);

    FTimerHandle& Handle = CancelTimers.FindOrAdd(SourceId);
    GetWorld()->GetTimerManager().SetTimer(
        Handle,
        FTimerDelegate::CreateWeakLambda(this, [this, SourceId, Reason]()
        {
            // SourceId가 일치하는 이동만 취소
            CancelBySourceId(SourceId, Reason);
        }),
        Seconds,
        false
    );
}

void UMoveUtilComponent::ClearCancelTimer(int32 SourceId)
{
    if (!GetWorld()) return;
    if (FTimerHandle* Handle = CancelTimers.Find(SourceId))
    {
        GetWorld()->GetTimerManager().ClearTimer(*Handle);
        CancelTimers.Remove(SourceId);
    }
}
