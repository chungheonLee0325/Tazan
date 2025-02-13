// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Yetuga.generated.h"

class APlayer_Kazan;

UENUM(BlueprintType)
enum class EYetugaAnimType : uint8
{
	MovingAtk		UMETA(DisplayName = "MovingAtk"),
	SweapAtk		UMETA(DisplayName = "SweepAtk"),
	TurnAtk			UMETA(DisplayName = "TurnAtk"),
	FastBallAtk		UMETA(DisplayName = "FastBallAtk"),
	Roar			UMETA(DisplayName = "Roar"),
	BackMove		UMETA(DisplayName = "BackMove"),
	NormalAtk		UMETA(DisplayName = "NormalAtk"),
	BackAtk			UMETA(DisplayName = "BackAtk")
};

UCLASS()
class TAZAN_API AYetuga : public ABaseMonster
{
	GENERATED_BODY()

public:
	AYetuga();
	APlayer_Kazan* GetPlayer_Kazan() const {return Player;}

	UPROPERTY(EditAnywhere, Category = "Skill Anim | Weaving Skill")
	TMap<EYetugaAnimType, TObjectPtr<UAnimMontage>> AnimMontageMap;

private:
	UPROPERTY()
	APlayer_Kazan* Player;
		
protected:
	virtual void BeginPlay() override;
	virtual UBaseAiFSM* CreateFSM() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	float DistToPlayer();
	float GetPlayerDir();
	
	UAnimMontage* GetAnimMontage(EYetugaAnimType animType); 
	
	void ShortAttack();
	void LongAtk();
};
