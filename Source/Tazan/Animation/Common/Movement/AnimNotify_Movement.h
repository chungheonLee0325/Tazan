#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "AnimNotify_Movement.generated.h"

UENUM(BlueprintType)
enum class EAreaMoveNotifyKind : uint8
{
    InFacing_Distance,  // Distance + Speed
    InFacing_Duration,  // Speed + Duration
    TowardsActor_Timed  // StopDistance + Speed + MaxDuration
};

UCLASS(meta=(DisplayName="Movement Utility : Movement Notify"))
class TAZAN_API UAnimNotify_Movement : public UAnimNotify
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mode")
    EAreaMoveNotifyKind Mode = EAreaMoveNotifyKind::InFacing_Distance;

    // 공통
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move")
    bool bStickToGround = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move")
    EMovementInterpolationType Interp = EMovementInterpolationType::EaseOut;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Priority")
    int32 Priority = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Priority")
    int32 SourceId = 1;

    // InFacing_Distance
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Distance", meta=(EditCondition="Mode==EAreaMoveNotifyKind::InFacing_Distance"))
    float Distance = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Distance", meta=(EditCondition="Mode==EAreaMoveNotifyKind::InFacing_Distance"))
    float Speed_Dist = 2400.f;

    // InFacing_Duration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Duration", meta=(EditCondition="Mode==EAreaMoveNotifyKind::InFacing_Duration"))
    float Speed_Time = 2400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InFacing|Duration", meta=(EditCondition="Mode==EAreaMoveNotifyKind::InFacing_Duration"))
    float Duration = 0.2f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Target", meta=(EditCondition="Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    float StopDistance = 120.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Target", meta=(EditCondition="Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    float Speed_Toward = 2400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Toward|Target", meta=(EditCondition="Mode==EAreaMoveNotifyKind::TowardsActor_Timed"))
    float MaxDuration = 0.25f;

    virtual FString GetNotifyName_Implementation() const override { return TEXT("Area Move Ex"); }
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference) override;
};
