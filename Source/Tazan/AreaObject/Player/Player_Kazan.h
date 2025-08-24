// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/Animation/Player/KazanAnimInstance.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Player_Kazan.generated.h"

class ULockOnComponent;
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
	STAGGER,
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
	bool bIsGuard;
	bool bIsSprinting = false;
	int HP_Potion_Max_Stack = 3;

	void SetPlayerState(EPlayerState NewState);
	void SetPlayerNormalState() { SetPlayerState(EPlayerState::NORMAL); }
	void SetComboState(bool bCanCombo, int SkillID);

	void SetHPRecoverMax();
	void DecreaseHPRecoverStack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnDie() override;

	virtual void OnRevival() override;

public:
	// Movement
	/** Called for movement input */
	void Move(FVector2D MovementVector);

	// Camera Rotation
	/** Called for looking input */
	void Look(FVector2D LookAxisVector);

	// Guard / Parry
	/** Called for guard input */
	void Guard_Pressed();
	void Guard_Released();
	void TryEndGuard();

	/** Called for attack input */
	void Attack_Weak_Pressed();
	void Attack_Strong_Pressed();
	void Attack_Strong_Triggered();
	void Attack_Strong_Released();
	void ChargeTimerCallback();

	/** Called for evade input */
	void Dodge_Pressed();

	/** Called for Sprint input */
	void On_Sprint_Pressed();
	void On_Sprint_Triggered();
	void On_Sprint_Released();

	/** Called for HPRecover input */
	void HPRecover_Pressed();

	/** Called for Restart input */
	void Restart_Pressed();

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SaveCheckpoint(FVector Location, FRotator Rotation);

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void RespawnAtCheckpoint();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* GuardReactionMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* PerfectGuardReactionMontage;

	virtual void HandlePerfectDodge() override;
	virtual void HandleGuard(AActor* DamageCauser, const FVector& HitLocation, const FAttackData& Data) override;
	virtual void HandlePerfectGuard(AActor* DamageCauser, const FVector& HitLocation, const FAttackData& Data) override;
	virtual void HandleGroggy(float Duration) override;

 	virtual void HandleStaggerBegin(EStaggerType Type, const FName& Direction) override;
	virtual void HandleStaggerEnd() override;

	void Reward(FItemData* ItemData, int ItemValue) const;

	UFUNCTION(BlueprintCallable)
	void RotateCameraWithSpeed(FRotator TargetRotate, float InterpSpeed = 10.f);
	void ZoomCameraWithSpeed(float TargetSize, float ZoomSpeed);

	void HandlePlayerCamera(AAreaObject* TargetAreaObject, FRotator TargetRotate, float RotateSpeed, float TargetSize,
	                        float ZoomSpeed);

	ULockOnComponent* GetLockOnComponent() const { return LockOnComponent; }

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
	
	// 플레이어 상태 관리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EPlayerState CurrentPlayerState;

	UPROPERTY(EditDefaultsOnly, Category = "State")
	TMap<EPlayerState, FActionRestrictions> StateRestrictions;

	void InitializeStateRestrictions();
	bool CanPerformAction(EPlayerState State, FString ActionName);

	// 가드 상태 변경 시 호출
	virtual void SetGuardState(bool bIsGuarding) override;

	FTimerHandle DodgeTimerHandle;

	// Data
	const float MAX_WALK_SPEED = 500.f;
	const float SPRINT_SPEED_RATIO = 1.8f;
	const float SPRINT_COST = 0.5f;
	const float MAX_GUARD_WALK_SPEED = 200.f;

	// ----- Charge Attack limits -----
	// 유지 최저치
	UPROPERTY(EditDefaultsOnly, Category="Sprint")
	float ChargeAttackMaintainMinStamina = 10.f;


	// ----- Sprint limits -----
	// 시작 최소치
	UPROPERTY(EditDefaultsOnly, Category="Sprint")
	float SprintStartMinStamina = 10.f;
	// 유지 최저치
	UPROPERTY(EditDefaultsOnly, Category="Sprint")
	float SprintMaintainMinStamina = 0.1f;

	// 가드 관련 변수들    
	// 최소 가드 지속 시간
	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	float MinGuardDuration = 0.5f;
	// 가드 해제 요청 상태
	bool bIsGuardRequested = false;
	FTimerHandle GuardMinDurationTimer;

	bool CanCombo;
	int NextComboSkillID = 0;

	float MaxChargeTime = 2.17f;
	float CurrentChargeTime = 0.0f;
	bool bIsCharging = false;
	FTimerHandle ChargeTimerHandle;

	bool IsRotateCameraWithSpeed;
	FRotator RotateCameraTarget;
	float CameraInterpSpeed = 10.f;
	FTimerHandle RotateCameraTimerHandle;

	bool IsZoomCameraWithSpeed;
	float TargetFieldOfView;
	float ZoomInterpSpeed;
	FTimerHandle ZoomCameraTimerHandle;

	// 락온 관련 이동/회전
	void UpdateLockedRotation(float DeltaTime);

	UPROPERTY(EditDefaultsOnly)
	ULockOnComponent* LockOnComponent;

	UPROPERTY()
	int HPRecoverStack = 3;

	// 마지막 체크포인트 위치
	UPROPERTY(VisibleAnywhere, Category = "Checkpoint")
	FVector LastCheckpointLocation = FVector::ZeroVector;

	// 마지막 체크포인트 회전
	UPROPERTY(VisibleAnywhere, Category = "Checkpoint")
	FRotator LastCheckpointRotation = FRotator::ZeroRotator;
};
