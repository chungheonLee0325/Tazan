#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CameraShakeNotifyState.generated.h"

class UCameraShakeBase;

UCLASS()
class TAZAN_API UCameraShakeNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	// 공통 카메라 셰이크 애셋
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	// 기본 Scale 값 (모든 애니메이션에 대해 조정 가능한 기본 값)
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	float ShakeScale = 1.0f;

	// 애니메이션별로 조정 가능한 추가 파라미터들
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	float BlendInTime = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	float BlendOutTime = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Camera Shake|Oscillation")
	float LocOscillationAmplitude = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Shake|Oscillation")
	float LocOscillationFrequency = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Shake|Oscillation")
	float RotOscillationAmplitude = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Shake|Oscillation")
	float RotOscillationFrequency = 15.0f;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
