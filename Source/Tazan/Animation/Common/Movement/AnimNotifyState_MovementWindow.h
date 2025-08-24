#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotify_Movement.h"              // EAreaMoveNotifyKind
#include "Tazan/AreaObject/Utility/MoveUtilComponent.h" // FAreaMoveSpec/Update
#include "AnimNotifyState_MovementWindow.generated.h"

UCLASS(meta=(DisplayName="Area: Move Window (Spec)"))
class TAZAN_API UAnimNotifyState_MovementWindow : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mode")
    EAreaMoveNotifyKind Mode = EAreaMoveNotifyKind::TowardsActor_Timed;

    // 프리셋 or 커스텀
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Advanced")
    bool bUseCustomSpec = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Advanced", meta=(EditCondition="bUseCustomSpec==true"))
    FAreaMoveSpec CustomSpec;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Advanced", meta=(EditCondition="bUseCustomSpec==true"))
    bool bAutoResolveTargetsForSpec = true;

    // 공통
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Common", meta=(EditCondition="bUseCustomSpec==false"))
    EMovementInterpolationType Interp = EMovementInterpolationType::EaseOut;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Common", meta=(EditCondition="bUseCustomSpec==false"))
    bool bStickToGround = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Common", meta=(EditCondition="bUseCustomSpec==false"))
    bool bSlideOnBlock = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Common", meta=(EditCondition="bUseCustomSpec==false"))
    int32 Priority = 100;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Common", meta=(EditCondition="bUseCustomSpec==false"))
    int32 SourceId = 2;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Common", meta=(EditCondition="bUseCustomSpec==false"))
    TObjectPtr<UCurveFloat> Curve = nullptr;

    // InFacing preset
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Distance",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::InFacing_Distance"))
    float Distance = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Duration",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::InFacing_Duration"))
    float Speed_Time = 2400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Duration",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::InFacing_Duration"))
    float Duration = 0.20f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Direction",
        meta=(EditCondition="bUseCustomSpec==false && (Mode==EAreaMoveNotifyKind::InFacing_Distance || Mode==EAreaMoveNotifyKind::InFacing_Duration)"))
    ERelMoveDir FacingDir = ERelMoveDir::Forward;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Direction",
        meta=(EditCondition="bUseCustomSpec==false && (Mode==EAreaMoveNotifyKind::InFacing_Distance || Mode==EAreaMoveNotifyKind::InFacing_Duration) && FacingDir==ERelMoveDir::CustomYaw"))
    float YawOffsetDeg = 0.f;

    // InFacing에서 FacingDir=Target일 때 타깃 해석
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Direction",
        meta=(EditCondition="bUseCustomSpec==false && (Mode==EAreaMoveNotifyKind::InFacing_Distance || Mode==EAreaMoveNotifyKind::InFacing_Duration) && FacingDir==ERelMoveDir::Target"))
    bool bFacingUseLockOnTarget = true;

    // Toward preset
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Target",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    bool bUseLockOnTarget = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Policy",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    ETowardPolicy TowardPolicy = ETowardPolicy::ReachStopDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Target",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::TowardsActor_Timed && TowardPolicy==ETowardPolicy::ReachStopDistance"))
    float StopDistance = 120.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Policy",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::TowardsActor_Timed && TowardPolicy==ETowardPolicy::FixedTravel"))
    float TravelDistance = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Target",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    float Speed_Toward = 2400.f;

    // 창 옵션
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Window") bool bStartOnBegin = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Window",
        meta=(EditCondition="Mode==EAreaMoveNotifyKind::TowardsActor_Timed")) bool bUpdateTargetEveryTick = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Window") bool bStopOnEnd = true;
    
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                            float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                           const FAnimNotifyEventReference& EventReference) override;
};
