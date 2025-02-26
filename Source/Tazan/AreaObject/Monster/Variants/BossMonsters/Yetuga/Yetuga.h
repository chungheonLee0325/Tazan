// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Yetuga.generated.h"

class UYetugaAnimInstance;
class UY_BaseSkill;
class UY_SelectSkill;
class APlayer_Kazan;

UCLASS()
class TAZAN_API AYetuga : public ABaseMonster
{
	GENERATED_BODY()

public:
	AYetuga();

	UPROPERTY()
	UYetugaAnimInstance* YetugaABP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =  "Animation | ChargeStun")
	UAnimMontage* ChargeStunAni;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =  "Animation | Recover")
	UAnimMontage* RecoverAni;

	// UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPlayerStatusWidget> StatusWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> MissionCompleteClass;

	UPROPERTY()
	class UPlayerStatusWidget* StatusWidget;

private:
	// UI
	
	UPROPERTY()
	class UUserWidget* CompleteWidget;
		
protected:
	virtual void BeginPlay() override;
	virtual UBaseAiFSM* CreateFSM() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION()
	void OnYetugaHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	virtual bool IsWeakPointHit(const FVector& HitLoc) override;
	virtual void ParryStackPenalty() override;
	virtual void HandleGroggy(float Duration) override;
	virtual void OnGroggyEnd() override;
	virtual void OnDie() override;
	
	TSet<int> GetSkillInstancesID() const {return m_OwnSkillIDSet;}

	
	UFUNCTION(BlueprintCallable)
	void ChangeStateToSelectSkill() { m_AiFSM->ChangeState(EAiStateType::SelectSkill); }
	UFUNCTION(BlueprintCallable)
	void ChangeStateToAttack() { m_AiFSM->ChangeState(EAiStateType::Attack); }
	
	UFUNCTION(BlueprintCallable)
	void ChargeSkillStun();
	UFUNCTION(BlueprintCallable)
	void Recover();

private:
	void InitializeHUD();
	
};
