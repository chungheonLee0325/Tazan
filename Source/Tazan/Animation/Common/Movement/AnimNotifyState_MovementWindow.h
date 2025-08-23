// AnimNotifyState_AreaMoveWindow.h
#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotify_Movement.h"
#include "AnimNotifyState_MovementWindow.generated.h"

UCLASS(meta=(DisplayName="Movement Utility : Movement Window NotifyState"))
class TAZAN_API UAnimNotifyState_MovementWindow : public UAnimNotifyState
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mode")
    EAreaMoveNotifyKind Mode = EAreaMoveNotifyKind::TowardsActor_Timed;

    // 공통
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move")
    bool bStickToGround = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move")
    EMovementInterpolationType Interp = EMovementInterpolationType::EaseOut;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Priority")
    int32 Priority = 100;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Priority")
    int32 SourceId = 2;

    // 파라미터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Distance", meta=(EditCondition="Mode==EAreaMoveNotifyKind::InFacing_Distance"))
    float Distance = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Duration", meta=(EditCondition="Mode==EAreaMoveNotifyKind::InFacing_Duration"))
    float Speed_Time = 2400.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Duration", meta=(EditCondition="Mode==EAreaMoveNotifyKind::InFacing_Duration"))
    float Duration = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Target", meta=(EditCondition="Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    bool bUseLockOnTarget = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Target", meta=(EditCondition="Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    float StopDistance = 120.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Target", meta=(EditCondition="Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    float Speed_Toward = 2000.f;

    // 동작 옵션
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Window")
    bool bStartOnBegin = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Window", meta=(EditCondition="Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    bool bUpdateTargetEveryTick = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Window")
    bool bStopOnEnd = true;

    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
