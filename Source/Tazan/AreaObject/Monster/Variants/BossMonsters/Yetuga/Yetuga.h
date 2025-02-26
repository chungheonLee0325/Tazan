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
	
	/**차지 어택 스턴시 애니메이션*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =  "Animation | ChargeStun")
	UAnimMontage* ChargeStunAni;

	// UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPlayerStatusWidget> StatusWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> MissionCompleteClass;

private:
	// UI
	UPROPERTY()
	class UPlayerStatusWidget* StatusWidget;
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
	
	virtual void OnDie() override;
	
	TSet<int> GetSkillInstancesID() const {return m_OwnSkillIDSet;}

	UFUNCTION(BlueprintCallable)
	void ChangeStateToSelectSkill() { m_AiFSM->ChangeState(EAiStateType::SelectSkill); }
	UFUNCTION(BlueprintCallable)
	void ChangeStateToAttack() { m_AiFSM->ChangeState(EAiStateType::Attack); }

	//특수 스킬용
	void ChargeSkillStun();

private:
	void InitializeHUD();
	
};
