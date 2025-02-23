// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/Animation/Player/KazanAnimInstance.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Player_Kazan.generated.h"

class AKazanPlayerController;
class ABaseItem;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

// 플레이어의 상태를 정의하는 열거형
UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	// 일반 상태
	NORMAL,
	// 회전만 가능한 상태 (공격 Casting 중 1tick)
	ONLY_ROTATE,
	// Action 상태
	ACTION,
	// Guard 상태
	GUARD,
	// Action 사용 가능한 상태
	CANACTION,
	// 경직 상태
	STAGER,
	// 사망 상태
	DIE,
};

// 액션 제한을 관리하는 구조체
USTRUCT(BlueprintType)
struct FActionRestrictions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanLook = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRotate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanOnlyRotate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanAction = true;
};

UCLASS()
class TAZAN_API APlayer_Kazan : public AAreaObject
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayer_Kazan();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Special
	UFUNCTION(BlueprintCallable)
	void SpecialFUNCTION();
	bool IsSpecial = false;
	FTimerHandle SpecialTimerHandle;

	void SetPlayerState(EPlayerState NewState);
	void SetPlayerNormalState() { SetPlayerState(EPlayerState::NORMAL); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnDie() override;

public:
	// Movement
	/** Called for movement input */
	void Move(FVector2D MovementVector);

	// Camera Rotation
	/** Called for looking input */
	void Look(FVector2D LookAxisVector);

	// Guard / Parry
	/** Called for parry input */
	void Guard_Pressed();
	void Guard_Released();

	/** Called for attack input */
	void Attack_Weak_Pressed();
	void Attack_Strong_Pressed();

	/** Called for evade input */
	void Dodge_Pressed();

	/** Called for run input */
	void On_Run_Pressed();
	void On_Run_Released();

	virtual void HandlePerfectDodge() override;

	void Reward(FItemData* ItemData, int ItemValue) const;

private:
	// Weapon Setting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponComponent;

	// Camera Setting
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	// 카메라 민감도 설정
	UPROPERTY(EditAnywhere, Category = "Camera|Controls")
	float LookSensitivityX = 0.25f;
	UPROPERTY(EditAnywhere, Category = "Camera|Controls")
	float LookSensitivityY = 0.15f;
	// 수직 각도 제한
	UPROPERTY(EditAnywhere, Category = "Camera|Limits")
	float MinPitchAngle = -10.0f; // 위쪽 제한
	UPROPERTY(EditAnywhere, Category = "Camera|Limits")
	float MaxPitchAngle = 40.0f; // 아래쪽 제한
	// 현재 피치 각도를 추적
	float CurrentPitchAngle = 0.0f;

	UPROPERTY()
	UKazanAnimInstance* KazanAnimInstance;
	UPROPERTY()
	AKazanPlayerController* KazanPlayerController;

	// UPROPERTY(EditDefaultsOnly, Category = "Animation")
	// UAnimMontage* DodgeAnimMontage;
	// UPROPERTY(EditDefaultsOnly, Category = "Animation")
	// UAnimMontage* BackDodgeAnimMontage;

	// 플레이어 상태 관리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EPlayerState CurrentPlayerState;

	UPROPERTY(EditDefaultsOnly, Category = "State")
	TMap<EPlayerState, FActionRestrictions> StateRestrictions;

	void InitializeStateRestrictions();
	bool CanPerformAction(EPlayerState State, FString ActionName);


	// 가드 상태 변경 시 호출
	virtual void SetGuardState(bool bIsGuarding) override;
	//virtual void SetActionState(bool bIsAction);

	//bool CanDodge = true;
	//float DodgeCoolTime = 1.0f;
	//UPROPERTY(EditDefaultsOnly, Category = "STATUS")
	//float DODGE_COST = 30.f;

	//bool CanAction = true;
	//bool CanMove = true;
	//bool CanOnlyRotate = false;

	//TMap<EActionAbility, bool> ActionAbilityMap;

	FTimerHandle DodgeTimerHandle;

	// Data
	const float MAX_WALK_SPEED = 500.f;
	const float MAX_GUARD_WALK_SPEED = 200.f;

	UPROPERTY(EditAnywhere, Category = "Monster Settings")
	TArray<UMaterialInterface*> GhostTrailMaterials;
};
