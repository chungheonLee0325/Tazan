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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int StartSkillID = 11000;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMeshComponent* SmallRockMesh;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AYetuga_RockS> SmallRock;
	
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

	FVector StartPelvisWS;
	FVector PrevPelvisWS;
	bool bIsAnimMoving = false;
		
protected:
	virtual void BeginPlay() override;
	virtual UBaseAiFSM* CreateFSM() override;

	virtual UBaseSkillRoulette* CreateSkillRoulette() override;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	UFUNCTION(BlueprintCallable)
	void ChangeStateToGroggy() { m_AiFSM->ChangeState(EAiStateType::Idle); }

	void OnStun(UAnimMontage* AnimMontage, bool bArg);
	// 스킬 관련
	UFUNCTION(BlueprintCallable)
	void ChargeSkillStun();
	
	UFUNCTION(BlueprintCallable)
	void Recover();
	
	UFUNCTION(BlueprintCallable)
	void FastBall();
	void FastBallDestroy();
	
	UFUNCTION(BlueprintCallable)
	void ShowRock();
	UFUNCTION(BlueprintCallable)
	void HideRock();

	UFUNCTION(BlueprintCallable)
	void StartAnimMove();
	UFUNCTION(BlueprintCallable)
	void EndAnimMove();
	UFUNCTION(BlueprintCallable)
	void AnimMove();

private:
	virtual void InitializeHUD() override;
	
};
