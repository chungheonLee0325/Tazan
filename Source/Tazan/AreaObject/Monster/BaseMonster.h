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
#include "Tazan/Utilities/LogMacro.h"
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
	virtual AAreaObject* GetAggroTarget() const;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetDistToTarget();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FVector GetDirToTarget();
	
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
	virtual void SetAggroTarget(AAreaObject* NewTarget) { m_AggroTarget = NewTarget; }

	// Data Access
	const FMonsterData& GetMonsterData() const { return MonsterData; }

	// Monster Data
	UPROPERTY(EditAnywhere, Category = "Monster Settings")
	FMonsterData MonsterData;

	UPROPERTY()
	FTimerHandle OnDieHandle;
	
	UBaseAiFSM* GetFSM() const {return m_AiFSM;}

protected:
	virtual void OnDie() override;

	UFUNCTION(BlueprintCallable)

	virtual UBaseAiFSM* CreateFSM();
	
	// Combat System
	UPROPERTY()
	AAreaObject* m_AggroTarget;

	UPROPERTY()
	FVector m_SpawnLocation;

	UPROPERTY()
	UBaseAiFSM* m_AiFSM;

private:
	UPROPERTY()
	AActor* m_CurrentTarget;
};
