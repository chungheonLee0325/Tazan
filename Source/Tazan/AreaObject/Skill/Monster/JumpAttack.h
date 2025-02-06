// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"
#include "JumpAttack.generated.h"


UCLASS()
class TAZAN_API UJumpAttack : public UBaseSkill
{
	GENERATED_BODY()

public:
	UJumpAttack();

protected:
	virtual void OnPhaseChanged(ESkillPhase NewPhase) override;
	virtual void UpdatePreparePhase(float DeltaTime) override;
	virtual void UpdateCastingPhase(float DeltaTime) override;
	virtual auto UpdatePostCastPhase(float DeltaTime) -> void override;
	

private:
	int RepeatCount = 2;
	float Radian = 0.0f;
	float Amplitue = 0.3f;
	FRotator Rotation = FRotator(0.0f, 0.f,60.f);
	float Period = 8.f;

	float JumpRange = 800.f;
};
