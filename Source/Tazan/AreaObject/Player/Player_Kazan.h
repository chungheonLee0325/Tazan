// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Player_Kazan.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Movement
	/** Called for movement input */
	void Move(FVector2D MovementVector);

	// Rotation
	/** Called for looking input */
	void Look(FVector2D LookAxisVector);

private:
	/** Called for attack input */
	void On_Attack_Common_Pressed();
	void On_Attack_Strong_Pressed();
	/** Called for parry input */
	void On_Parry_Pressed();
	void On_Parry_Released();
	/** Called for evade input */
	void On_Evade_Pressed();
	/** Called for run input */
	void On_Run_Pressed();
	void On_Run_Released();

	// Animation Notify Handlers
	UFUNCTION(BlueprintCallable)
	void OnParryActivated();

	UFUNCTION(BlueprintCallable)
	void OnParryDeactivated();

	UFUNCTION(BlueprintCallable)
	void OnPerfectParryActivated();

	UFUNCTION(BlueprintCallable)
	void OnPerfectParryDeactivated();

	UFUNCTION(BlueprintCallable)
	void OnAttackHitStart();

	UFUNCTION(BlueprintCallable)
	void OnAttackHitEnd();

	UFUNCTION(BlueprintCallable)
	void OnDodgeInvincibilityStart();

	UFUNCTION(BlueprintCallable)
	void OnDodgeInvincibilityEnd();

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
};
