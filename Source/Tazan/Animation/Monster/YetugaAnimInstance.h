// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/Animation/Common/BaseAnimInstance.h"
#include "YetugaAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EYAnimState : uint8
{
	None,
	ParryGroggyEnter,
	NormalGroggyEnter,
	ChargeGroggy,
	GroggyProcess,
	GroggyEnd
};

class AYetuga;

UCLASS()
class TAZAN_API UYetugaAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	EYAnimState CurrentAnimState = EYAnimState::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	float GroggyDuration = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParryGroggy | BlendSpace")
	float BlendDuration = 1.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParryGroggy | BlendSpace")
	float BlendValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AYetuga* Yetuga;

private:
	float curTime = 0.0f;
	
protected:
	virtual void NativeUpdateAnimation(float dt) override;;

	void GroggyInProgress(float dt);
	void ParryEntering(float dt);
	void ChargeEntering();
	void GroggyEnd();

};
