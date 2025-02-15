// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Yetuga.generated.h"

class UY_SkillRoulette;
class UY_SelectSkill;
class APlayer_Kazan;

UENUM(BlueprintType)
enum class EWeavingSkillAnim : uint8
{
	MovingAtk		UMETA(DisplayName = "MovingAtk"),
	SweapAtk		UMETA(DisplayName = "SweepAtk"),
	TurnAtk			UMETA(DisplayName = "TurnAtk"),
	BackMove		UMETA(DisplayName = "BackMove"),
	ShortMoveR 		UMETA(DisplayName = "ShortMoveR"),
	ShortMoveAtkR 	UMETA(DisplayName = "ShortMoveAtkR"),
	ShortMoveL 		UMETA(DisplayName = "ShortMoveL"),
	ShortMoveAtkL 	UMETA(DisplayName = "ShortMoveAtkL"),
	UpperCut		UMETA(DisplayName = "UpperCut"),
	BackAtk			UMETA(DisplayName = "BackAtk")
};

UCLASS()
class TAZAN_API AYetuga : public ABaseMonster
{
	GENERATED_BODY()

public:
	AYetuga();

	UPROPERTY(EditAnywhere, Category = "Skill Anim | WeavingSkill")
	TMap<EWeavingSkillAnim, TObjectPtr<UAnimMontage>> AnimMontageMap;

	UPROPERTY()
	UY_SkillRoulette* SkillRoulette;

private:
	UPROPERTY()
	APlayer_Kazan* Player;
		
protected:
	virtual void BeginPlay() override;
	virtual UBaseAiFSM* CreateFSM() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//TODO: AreaObject로 이전?
	TSet<int> GetSkillInstancesID() const {return m_OwnSkillIDSet;}
	
	APlayer_Kazan* GetPlayer_Kazan() const {return Player;}
	float DistToPlayer();
	float GetPlayerDir();
	
	UAnimMontage* GetAnimMontage(EWeavingSkillAnim animType); 
	
	void ShortAttack();
	void LongAtk();

private:
	
};
