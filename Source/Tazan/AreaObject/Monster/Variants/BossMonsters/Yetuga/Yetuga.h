// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Yetuga.generated.h"

class AYetuga_RockS;
class UBoxComponent;
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
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AYetuga_RockS> SmallRock;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "Skill | ChargeAttack")
	UBoxComponent* ChargeStunCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =  "Animation | ChargeStun")
	UAnimMontage* ChargeStunAni;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =  "Animation | Recover")
	UAnimMontage* RecoverAni;

	// UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPlayerStatusWidget> BossStatusWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> MissionCompleteClass;

private:
	// UI
	UPROPERTY()
	class UUserWidget* CompleteWidget;
		
protected:
	virtual void BeginPlay() override;
	virtual UBaseAiFSM* CreateFSM() override;

	virtual UBaseSkillRoulette* CreateSkillRoulette() override;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION()
	void OnYetugaHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 오버라이드 함수
	virtual bool IsWeakPointHit(const FVector& HitLoc) override;
	
	virtual void ParryStackPenalty() override;
	
	virtual void HandleGroggy(float Duration) override;
	virtual void OnGroggyEnd() override;
	
	virtual void OnDie() override;
	
	TSet<int> GetSkillInstancesID() const {return m_OwnSkillIDSet;}

	// 전투 시작
	UFUNCTION(BlueprintCallable)
	void YetugaStart();

	// 상태 전이
	UFUNCTION(BlueprintCallable)
	void ChangeStateToSelectSkill() { m_AiFSM->ChangeState(EAiStateType::SelectSkill); }
	UFUNCTION(BlueprintCallable)
	void ChangeStateToAttack() { m_AiFSM->ChangeState(EAiStateType::Attack); }

	// 스킬 관련
	UFUNCTION(BlueprintCallable)
	void ChargeSkillStun();
	UFUNCTION(BlueprintCallable)
	void Recover();
	UFUNCTION(BlueprintCallable)
	void FastBall();

private:
	virtual void InitializeHUD() override;
	
};
