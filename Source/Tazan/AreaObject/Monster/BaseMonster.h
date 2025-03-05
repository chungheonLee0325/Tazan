/*
BaseMonster System
├── Core Features
│   ├── Movement System
│   │   ├── Movement
│   │   ├── Rotation
│   └── Combat System
│       ├── Skill Management
│       ├── Target Management
│       └── State Management
├── AI Features
│   ├── State Machine
│   │   ├── Idle
│   │   ├── Chase
│   │   ├── Attack
│   │   └── BackHome
│   └── Aggro System
│       ├── Target Selection
│       ├── Range Check
│       └── Aggro Table
└── Utility Features
    ├── Movement Interface
    │   ├── Actor Movement
    │   ├── Component Movement
    │   └── Rotation Control
    └── Combat Interface
        ├── Skill Usage
        ├── Target Selection
        └── State Changes
*/

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Containers/Queue.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Y_SkillRoulette.h"
#include "Tazan/Contents/TazanGameInstance.h"
#include "Tazan/Utilities/LogMacro.h"
#include "BaseMonster.generated.h"

class UWidgetComponent;
struct FAIStimulus;
class USkillBag;
class UBaseAiFSM;
class UBaseSkill;
class UPathMover;

UCLASS()
class TAZAN_API ABaseMonster : public AAreaObject
{
	GENERATED_BODY()

public:
	ABaseMonster();
	// Skill
	FSkillBagData* dt_SkillBag;

	UPROPERTY(BlueprintReadWrite)
	UBaseSkill* NextSkill;

	UFUNCTION()
	UBaseSkillRoulette* GetSkillRoulette() const;
	
	/// Perfect Gaurd 애니메이션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation | Parry")
	UAnimMontage* ParryPenaltyAnimation;

	// Perfect Gaurd
	bool bIsParrySkill = true;
	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	int ParryStackMax = 5;

	UPROPERTY(EditAnywhere, Category = "UI")
	UWidgetComponent* HPWidgetComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	class UPlayerStatusWidget* StatusWidget;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	float HeightHPUI = 160.0f;
	
	UPROPERTY()
	FTimerHandle OnDieHandle;

	virtual float DecreaseHP(float Delta) override;
	virtual float DecreaseStamina(float Delta, bool bIsDamaged = true) override;
	void SetHPWidgetVisibility(bool IsVisible);
	void SetHPWidgetVisibilityByDuration(float Duration);
	FTimerHandle HPWidgetVisibleTimer;
protected:
	// Combat System
	void HandleStaggerBegin(EStaggerType Type, const FName& Direction) override;
	virtual void HandleStaggerEnd() override;
	UPROPERTY()
	AAreaObject* m_AggroTarget;
	UPROPERTY()
	FVector m_SpawnLocation;
	UPROPERTY(VisibleAnywhere)
	UBaseAiFSM* m_AiFSM;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill")
	UBaseSkillRoulette* m_SkillRoulette;

	UPROPERTY(EditDefaultsOnly, Category = "Sight")
	float SightRadius = 1500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Sight")
	float LoseSightRadius = 1500.f;
	
	//퍼펙트 가드시 누적될 데미지
	UPROPERTY(VisibleDefaultsOnly, Category = "Parry")
	int ParryStack = 0;
	
private:
	UPROPERTY()
	AActor* m_CurrentTarget;

public:
	// Core Functions
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	virtual void OnBodyBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                                AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp,
	                                int32 OtherBodyIndex,
	                                bool bFromSweep,
	                                const FHitResult& SweepResult);

	// Combat Interface
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual AAreaObject* GetAggroTarget() const;

	// Combat System
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void SetAggroTarget(AAreaObject* NewTarget) { m_AggroTarget = NewTarget; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetDistToTarget();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FVector GetDirToTarget();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetNextSkillRange();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FVector GetSpawnLocation();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetSightLength();

	// State Checks
	UFUNCTION(BlueprintPure, Category = "State")
	bool IsMoving() const;
	UFUNCTION(BlueprintPure, Category = "State")
	bool IsRotating() const;

	// Skill
	void RemoveSkillEntryByID(const int id);
	void AddSkillEntryByID(const int id);

	// Perfect Guard Reaction
	virtual void AddParryStack();
	void InitParryStack();

	// AI Perception 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAIPerceptionComponent* AIPerceptionComponent;

	// 시야 설정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAISenseConfig_Sight* SightConfig;

	// 감지 이벤트를 처리할 함수
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

protected:
	// 가상 팩토리 함수
	UFUNCTION(BlueprintCallable)
	virtual UBaseAiFSM* CreateFSM();
	UFUNCTION(BlueprintCallable)
	virtual UBaseSkillRoulette* CreateSkillRoulette();

	virtual void OnDie() override;

	// Perfect Guard
	virtual void ParryStackPenalty();

	virtual void InitializeHUD();
};
