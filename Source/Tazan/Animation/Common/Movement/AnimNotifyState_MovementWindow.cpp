#include "AnimNotifyState_MovementWindow.h"

#include "Components/CapsuleComponent.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/AreaObject/Utility/MoveUtilComponent.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
#include "Tazan/AreaObject/Player/Component/LockOnComponent.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"


static void FillCommon(FAreaMoveSpec& S, const UAnimNotifyState_MovementWindow* N)
{
    S.Interp        = N->Interp;
    S.bStickToGround= N->bStickToGround;
    S.bSlideOnBlock = N->bSlideOnBlock;
    S.Priority      = N->Priority;
    S.SourceId      = N->SourceId;
    S.Curve         = N->Curve;
}

static void AutoResolveTargetsIfNeeded(FAreaMoveSpec& S, AAreaObject* Area, bool bUseLockOnToward, bool bUseLockOnFacing)
{
    if (!Area) return;

    if (S.Intent == EMoveIntent::TowardActor && !S.TowardActor.IsValid())
    {
        S.TowardActor = UMoveUtilComponent::ResolveAreaTarget(Area, bUseLockOnToward);
    }
    if (S.Intent == EMoveIntent::InFacing && S.FacingDir == ERelMoveDir::Target && !S.TargetActor.IsValid())
    {
        S.TargetActor = UMoveUtilComponent::ResolveAreaTarget(Area, bUseLockOnFacing);
    }
}

void UAnimNotifyState_MovementWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
    if (!bStartOnBegin || !MeshComp) return;

    if (AAreaObject* Area = Cast<AAreaObject>(MeshComp->GetOwner()))
    {
        // 커스텀 우선
        if (bUseCustomSpec)
        {
            FAreaMoveSpec S = CustomSpec;
            if (bAutoResolveTargetsForSpec)
            {
                AutoResolveTargetsIfNeeded(S, Area, /*toward*/true, /*facing*/true);
            }
            Area->StartMoveSpec(S);
            return;
        }

        FAreaMoveSpec S;

        switch (Mode)
        {
        case EAreaMoveNotifyKind::InFacing_Distance:
            S.Intent     = EMoveIntent::InFacing;
            S.Kinematics = EKinematics::BySpeed;
            S.Distance   = Distance;
            S.Speed      = Speed_Toward;     // 윈도우 공통 속도로 운용
            S.FacingDir  = FacingDir;
            S.YawOffsetDeg = YawOffsetDeg;
            FillCommon(S, this);
            if (FacingDir == ERelMoveDir::Target)
                S.TargetActor = UMoveUtilComponent::ResolveAreaTarget(Area, bFacingUseLockOnTarget);
            Area->StartMoveSpec(S);
            break;

        case EAreaMoveNotifyKind::InFacing_Duration:
            S.Intent     = EMoveIntent::InFacing;
            S.Kinematics = EKinematics::ByDuration;
            S.Speed      = Speed_Time;
            S.Duration   = Duration;
            S.FacingDir  = FacingDir;
            S.YawOffsetDeg = YawOffsetDeg;
            FillCommon(S, this);
            if (FacingDir == ERelMoveDir::Target)
                S.TargetActor = UMoveUtilComponent::ResolveAreaTarget(Area, bFacingUseLockOnTarget);
            Area->StartMoveSpec(S);
            break;

        case EAreaMoveNotifyKind::TowardsActor_Timed:
        {
            FAreaMoveSpec T;
            T.Intent       = EMoveIntent::TowardActor;
            T.Kinematics   = EKinematics::BySpeed;
            T.TowardActor  = UMoveUtilComponent::ResolveAreaTarget(Area, bUseLockOnTarget);
            if (!T.TowardActor.IsValid()) break;

            T.TowardPolicy = TowardPolicy;
            if (TowardPolicy == ETowardPolicy::ReachStopDistance)
                T.StopDistance = StopDistance;
            else
                T.TravelDistance = TravelDistance;

            T.Speed        = Speed_Toward;
            T.MaxDuration  = 0.f; // 윈도우 종료로만 멈추는 걸 권장

            FillCommon(T, this);
            Area->StartMoveSpec(T);
            break;
        }
        }
    }
}

void UAnimNotifyState_MovementWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
    if (!MeshComp) return;

    if (Mode == EAreaMoveNotifyKind::TowardsActor_Timed && bUpdateTargetEveryTick)
    {
        if (AAreaObject* Area = Cast<AAreaObject>(MeshComp->GetOwner()))
        {
            if (AActor* Target = UMoveUtilComponent::ResolveAreaTarget(Area, bUseLockOnTarget))
            {
                const FVector NewTarget = UMoveUtilComponent::ComputeTowardTargetPos(Area, Target, StopDistance);

                FAreaMoveUpdate U;
                U.SourceId = SourceId;
                U.bRetarget = true;
                U.NewTargetLocation = NewTarget;

                Area->UpdateMoveSpec(U);
            }
        }
    }
}

void UAnimNotifyState_MovementWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);
    if (!bStopOnEnd || !MeshComp) return;

    if (AAreaObject* Area = Cast<AAreaObject>(MeshComp->GetOwner()))
    {
        Area->StopMoveBySourceId(SourceId, EMoveFinishReason::Canceled);
    }
}
