/*
BaseMonster System
├── Core Features
│   ├── Monster Data
│   │   ├── Base Stats (TODO: DataTable)
│   │   ├── Current Stats
│   │   └── Combat Stats (TODO: DataTable)
│   ├── Movement System
│   │   ├── Movement
│   │   ├── Rotation
│   │   └── Component Movement
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
#include "BaseMonster.generated.h"

class UBaseAiFSM;
class UBaseSkill;
class UPathMover;

USTRUCT(BlueprintType)
struct FMonsterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(Comment="TODO: Move to DataTable"))
	float MaxHealth = 1.0f;

	UPROPERTY(EditAnywhere, meta=(Comment="TODO: Move to DataTable"))
	float DetectionRange = 1000.0f;

	UPROPERTY(EditAnywhere, meta=(Comment="TODO: Move to DataTable"))
	float MovementSpeed = 300.0f;

	UPROPERTY(EditAnywhere, meta=(Comment="TODO: Move to DataTable"))
	float RotationSpeed = 360.0f;
};

UCLASS()
class TAZAN_API ABaseMonster : public AAreaObject
{
	GENERATED_BODY()

public:
	ABaseMonster();

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
		const FHitResult & SweepResult);
	
	// Combat Interface
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual AActor* GetAggroTarget() const;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual bool CanCastSkill(UBaseSkill* Skill, const AActor* Target) const;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void CastSkill(UBaseSkill* Skill, const AActor* Target);

	// Skill Interface
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual UBaseSkill* GetCurrentSkill();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void UpdateCurrentSkill(UBaseSkill* NewSkill);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ClearCurrentSkill();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EnQueueSkill(UBaseSkill* Skill);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual UBaseSkill* DeQueueSkill();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ClearQueueSkill();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual UBaseSkill* FindSkillByClass(TSubclassOf<UBaseSkill> SkillClass);	// 아마 Skill Table 추가되면 SkillID로 찾는 방식으로 변화될듯
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	UBaseSkill* FindSkillByState(EAiStateType StateType) const;

	
	// State Checks
	UFUNCTION(BlueprintPure, Category = "State")
	bool IsMoving() const;
	
	UFUNCTION(BlueprintPure, Category = "State")
	bool IsRotating() const;

	// Control Interface
	UFUNCTION(BlueprintCallable, Category = "Control")
	void StopMoving();

	UFUNCTION(BlueprintCallable, Category = "Control")
	void StopRotating();

	UFUNCTION(BlueprintCallable, Category = "Control")
	void StopAll();

	// Combat System
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void SetAggroTarget(AActor* NewTarget) { m_AggroTarget = NewTarget; }

	// Data Access
	const FMonsterData& GetMonsterData() const { return MonsterData; }

	// Monster Data
	UPROPERTY(EditAnywhere, Category = "Monster Settings")
	FMonsterData MonsterData;

	UPROPERTY()
	FTimerHandle OnDieHandle;


protected:
	virtual void OnDie() override;

	UFUNCTION(BlueprintCallable)

	virtual UBaseAiFSM* CreateFSM();
	
	// Skill System
	UPROPERTY(EditAnywhere, Category = "Skill")
	TSet<TSubclassOf<UBaseSkill>> m_SkillClasses;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TMap<TSubclassOf<UBaseSkill>,UBaseSkill*> m_SkillInstances;
		
	UPROPERTY(EditAnywhere, Category = "Skill")
	TMap<EAiStateType, TSubclassOf<UBaseSkill>> m_StateSkillClasses;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TMap<EAiStateType, UBaseSkill*> m_StateSkillInstances;
	
	// Combat System
	UPROPERTY()
	AActor* m_AggroTarget;

	UPROPERTY()
	FVector m_SpawnLocation;

public:
	FVector GetM_SpawnLocation() const;

protected:
	UPROPERTY()
	UBaseAiFSM* m_AiFSM;

	UPROPERTY()
	UBaseSkill* m_CurrentSkill;
	
	TQueue<UBaseSkill*> m_SkillQueue;

	// Death Setting
	UPROPERTY(EditAnywhere, Category = "Death Settings")
	float DestroyDelayTime = 0.4f;
	
	UPROPERTY(EditAnywhere, Category = "Death Settings")
	UParticleSystem* DeathEffect;

private:
	UPROPERTY()
	AActor* m_CurrentTarget;
};
