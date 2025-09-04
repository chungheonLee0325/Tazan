// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Yetuga_RockS.generated.h"

class UProjectileMovementComponent;
class AAreaObject;
class ABaseMonster;

UCLASS()
class TAZAN_API AYetuga_RockS : public AActor
{
	GENERATED_BODY()

public:
	AYetuga_RockS();

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* Root;
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category="Destroy Animation")
	UAnimMontage* FloorDestroyAni;
	UPROPERTY(EditDefaultsOnly, Category="Destroy Animation")
	UAnimMontage* TargetDestroyAni;

private:
	UPROPERTY()
	ABaseMonster* Caster;
	UPROPERTY()
	AAreaObject* Target;

	bool bIsHitOnce = false;
	// FAttackCollision* AttackCollision;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void InitRock(ABaseMonster* caster, AAreaObject* player);

	void Fire();
	
	UFUNCTION()
	void DoDestroy();
};