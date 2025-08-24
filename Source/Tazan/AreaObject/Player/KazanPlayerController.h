// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player_Kazan.h"
#include "GameFramework/PlayerController.h"
#include "KazanPlayerController.generated.h"

struct FInputActionValue;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCurrencyChangeDelegate, ECurrencyType, CurrencyType, int,
                                               CurrencyValue, int, Delta);

/**
 * 
 */
UCLASS()
class TAZAN_API AKazanPlayerController : public APlayerController
{
	GENERATED_BODY()
	virtual void SetupInputComponent() override;

public:
	AKazanPlayerController();

	virtual void BeginPlay() override;

	void AddCurrency(ECurrencyType CurrencyType, int CurrencyValue);
	void RemoveCurrency(ECurrencyType CurrencyType, int CurrencyValue);
	int GetCurrencyValue(ECurrencyType CurrencyType);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCurrencyChangeDelegate OnCurrencyChange;

	UPROPERTY()
	class UUserWidget* FailWidget;

	class UPlayerStatusWidget* GetPlayerStatusWidget() const;
private:
	// UI 초기화 및 바인딩
	void InitializeHUD();

	// Input Action
	/** Called for movement input */
	void OnMove(const FInputActionValue& Value);
	/** Called for looking input */
	void OnLook(const FInputActionValue& Value);
	/** Called for attack input */
	void On_Attack_Weak_Pressed(const FInputActionValue& InputActionValue);
	void On_Attack_Strong_Pressed(const FInputActionValue& InputActionValue);
	void On_Attack_Strong_Released(const FInputActionValue& InputActionValue);
	void On_Attack_Strong_Triggered(const FInputActionValue& InputActionValue);
	/** Called for parry input */
	void On_Parry_Pressed(const FInputActionValue& InputActionValue);
	void On_Parry_Released(const FInputActionValue& InputActionValue);
	/** Called for evade input */
	void On_Dodge_Pressed(const FInputActionValue& InputActionValue);
	/** Called for sprint input */
	void On_Sprint_Pressed(const FInputActionValue& InputActionValue);
	void On_Sprint_Triggerd(const FInputActionValue& InputActionValue);
	void On_Sprint_Released(const FInputActionValue& InputActionValue);
	/** Called for skill input */
	void On_Skill_1_Pressed(const FInputActionValue& InputActionValue);
	void On_Skill_1_Triggered(const FInputActionValue& InputActionValue);
	void On_Skill_1_Released(const FInputActionValue& InputActionValue);

	void On_Skill_2_Pressed(const FInputActionValue& InputActionValue);
	void On_Skill_2_Triggered(const FInputActionValue& InputActionValue);
	void On_Skill_2_Released(const FInputActionValue& InputActionValue);

	void On_Skill_3_Pressed(const FInputActionValue& InputActionValue);
	void On_Skill_3_Triggered(const FInputActionValue& InputActionValue);
	void On_Skill_3_Released(const FInputActionValue& InputActionValue);
	/** Called for LockOn input */
	void On_LockOn_Pressed();
	/** Called for SwitchTarget input */
	void On_SwitchTarget_Triggered(const FInputActionValue& Value);
	/** Called for HPRecover input */
	void On_RecoverHP_Pressed(const FInputActionValue& Value);
	/** Called for HPRecover input */
	void On_Restart_Pressed(const FInputActionValue& Value);

	// Owner
	UPROPERTY(VisibleAnywhere)
	APlayer_Kazan* Kazan;

	// 재화 관련 데이터
	TMap<ECurrencyType, int> CurrencyValues;

	// UI 관련
	UPROPERTY()
	class UPlayerStatusWidget* StatusWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPlayerStatusWidget> StatusWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> MissionFailClass;

	// Input Setting
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/** Attack_C Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LBAction;

	/** Attack_S Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RBAction;

	/** Skill 1 Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Skill_1_Action;

	/** Skill 2 Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Skill_2_Action;

	/** Skill 3 Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Skill_3_Action;
	
	/** Parry Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GuardAction;

	/** Evade Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EvadeAction;

	/** LockOn Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LockOnAction;
	
	/** SwitchTarget Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchTargetAction;

	/** RecoverHP Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HPRecoverAction;

	/** Restart Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RestartAction;
};
