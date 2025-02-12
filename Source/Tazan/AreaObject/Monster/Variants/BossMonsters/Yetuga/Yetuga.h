// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Yetuga.generated.h"

class APlayer_Kazan;

UCLASS()
class TAZAN_API AYetuga : public ABaseMonster
{
	GENERATED_BODY()

public:
	AYetuga();
	APlayer_Kazan* GetPlayer_Kazan() const {return Player;}

	UPROPERTY(EditAnywhere, Category = "Skill Anim | Weaving Skill")
	TObjectPtr<UAnimMontage> amMovingAtk;
	UPROPERTY(EditAnywhere, Category = "Skill Anim | Weaving Skill")
	TObjectPtr<UAnimMontage> amSweapAtk;
	UPROPERTY(EditAnywhere, Category = "Skill Anim | Weaving Skill")
	TObjectPtr<UAnimMontage> amThrowRockAtk;
	UPROPERTY(EditAnywhere, Category = "Skill Anim | Weaving Skill")
	TObjectPtr<UAnimMontage> amRoar;

private:
	UPROPERTY()
	APlayer_Kazan* Player;
		
protected:
	virtual void BeginPlay() override;
	virtual UBaseAiFSM* CreateFSM() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void WeaveingAttack(uint8 idx);
	void ShortAttack();
	void LongAtk();
};
