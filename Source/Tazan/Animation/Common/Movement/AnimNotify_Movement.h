#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "Tazan/AreaObject/Utility/MoveUtilComponent.h" 
#include "AnimNotify_Movement.generated.h"

UENUM(BlueprintType)
enum class EAreaMoveNotifyKind : uint8
{
    // Distance + Speed_Dist (+ FacingDir)
    InFacing_Distance,
    // Speed_Time + Duration (+ FacingDir)
    InFacing_Duration,
    // Stop/Speed/MaxDuration (+ TowardPolicy)
    TowardsActor_Timed
};

UCLASS(meta=(DisplayName="Area: Move (Spec)"))
class TAZAN_API UAnimNotify_Movement : public UAnimNotify
{
    GENERATED_BODY()
public:
    // 모드(프리셋) 또는 커스텀 스펙
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mode")
    EAreaMoveNotifyKind Mode = EAreaMoveNotifyKind::InFacing_Distance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Advanced")
    bool bUseCustomSpec = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Advanced", meta=(EditCondition="bUseCustomSpec==true"))
    FAreaMoveSpec CustomSpec;

    // 커스텀 스펙에서 필요한 타깃이 비어 있을 때 자동 보완
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
    int32 SourceId = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Common", meta=(EditCondition="bUseCustomSpec==false"))
    TObjectPtr<UCurveFloat> Curve = nullptr;

    // InFacing (거리 기반)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Distance",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::InFacing_Distance"))
    float Distance = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Distance",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::InFacing_Distance"))
    float Speed_Dist = 2400.f;

    // InFacing 방향 옵션 (+Target)
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

    // InFacing (시간 기반)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Duration",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::InFacing_Duration"))
    float Speed_Time = 2400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Duration",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::InFacing_Duration"))
    float Duration = 0.20f;

    // Toward 
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Target",
        meta=(EditCondition="bUseCustomSpec==false && Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    float MaxDuration = 0.25f;

    virtual FString GetNotifyName_Implementation() const override { return TEXT("Area Move (Spec)"); }
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference) override;
};
