#include "AnimNotify_Movement.h"
#include "GameFramework/Character.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/AreaObject/Utility/MoveUtilComponent.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
#include "Tazan/AreaObject/Player/Component/LockOnComponent.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"

static void FillCommon(FAreaMoveSpec& S, const UAnimNotify_Movement* N)
{
    S.Interp        = N->Interp;
    S.bStickToGround= N->bStickToGround;
    S.bSlideOnBlock = N->bSlideOnBlock;
    S.Priority      = N->Priority;
    S.SourceId      = N->SourceId;
    S.Curve         = N->Curve;
}

void UAnimNotify_Movement::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    if (!MeshComp) return;
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    if (AAreaObject* Area = Cast<AAreaObject>(Owner))
    {
        // 고급: CustomSpec 우선
        if (bUseCustomSpec)
        {
            FAreaMoveSpec S = CustomSpec;
            if (bAutoResolveTargetsForSpec)
            {
                UMoveUtilComponent::AutoResolveTargetsIfNeeded(S, Area, /*toward*/true, /*facing*/true);
            }
            Area->StartMoveSpec(S);
            return;
        }

        // 프리셋
        FAreaMoveSpec S;

        switch (Mode)
        {
        case EAreaMoveNotifyKind::InFacing_Distance:
            S.Intent      = EMoveIntent::InFacing;
            S.Kinematics  = EKinematics::BySpeed;
            S.Distance    = Distance;
            S.Speed       = Speed_Dist;
            S.FacingDir   = FacingDir;
            S.YawOffsetDeg= YawOffsetDeg;
            FillCommon(S, this);
            if (FacingDir == ERelMoveDir::Target)
                S.TargetActor = UMoveUtilComponent::ResolveAreaTarget(Area, bFacingUseLockOnTarget);
            Area->StartMoveSpec(S);
            return;

        case EAreaMoveNotifyKind::InFacing_Duration:
            S.Intent      = EMoveIntent::InFacing;
            S.Kinematics  = EKinematics::ByDuration;
            S.Speed       = Speed_Time;   // 거리 = 속도 * 시간
            S.Duration    = Duration;
            S.FacingDir   = FacingDir;
            S.YawOffsetDeg= YawOffsetDeg;
            FillCommon(S, this);
            if (FacingDir == ERelMoveDir::Target)
                S.TargetActor = UMoveUtilComponent::ResolveAreaTarget(Area, bFacingUseLockOnTarget);
            Area->StartMoveSpec(S);
            return;

        case EAreaMoveNotifyKind::TowardsActor_Timed:
        {
            S.Intent       = EMoveIntent::TowardActor;
            S.Kinematics   = EKinematics::BySpeed;
            S.TowardActor  = UMoveUtilComponent::ResolveAreaTarget(Area, bUseLockOnTarget);
            if (!S.TowardActor.IsValid()) return;

            S.TowardPolicy = TowardPolicy;
            if (TowardPolicy == ETowardPolicy::ReachStopDistance)
            {
                S.StopDistance = StopDistance;
            }
            else
            {
                S.TravelDistance = TravelDistance;
                // FixedTravel은 보통 타임아웃 비권장 (윈도우/씬 정책에 맞춰 직접 설정 가능)
            }

            S.Speed        = Speed_Toward;
            S.MaxDuration  = MaxDuration;

            FillCommon(S, this);
            Area->StartMoveSpec(S);
            return;
        }
        }
    }

    // 폴백: AAreaObject가 아닌 경우
    if (ACharacter* Ch = Cast<ACharacter>(Owner))
    {
        if (UMoveUtilComponent* MU = Ch->FindComponentByClass<UMoveUtilComponent>())
        {
            if (bUseCustomSpec) { MU->StartMove(CustomSpec); return; }

            FAreaMoveSpec S;
            switch (Mode)
            {
            case EAreaMoveNotifyKind::InFacing_Distance:
                S.Intent = EMoveIntent::InFacing; S.Kinematics = EKinematics::BySpeed;
                S.Distance = Distance; S.Speed = Speed_Dist; S.FacingDir = FacingDir; S.YawOffsetDeg = YawOffsetDeg;
                FillCommon(S, this); MU->StartMove(S); return;

            case EAreaMoveNotifyKind::InFacing_Duration:
                S.Intent = EMoveIntent::InFacing; S.Kinematics = EKinematics::ByDuration;
                S.Speed = Speed_Time; S.Duration = Duration; S.FacingDir = FacingDir; S.YawOffsetDeg = YawOffsetDeg;
                FillCommon(S, this); MU->StartMove(S); return;

            case EAreaMoveNotifyKind::TowardsActor_Timed:
                return; // 폴백에선 타깃 해석 미지원
            }
        }
    }
}
