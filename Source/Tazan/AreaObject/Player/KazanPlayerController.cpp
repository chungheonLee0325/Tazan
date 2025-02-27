// Fill out your copyright notice in the Description page of Project Settings.


#include "KazanPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LockOnComponent.h"
#include "Player_Kazan.h"
#include "Tazan/AreaObject/Attribute/StaminaComponent.h"
#include "Tazan/UI/Widget/PlayerStatusWidget.h"
#include "Tazan/Utilities/LogMacro.h"

AKazanPlayerController::AKazanPlayerController()
{
	// Enhanced Input Setting
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> tempInputMapping(
		TEXT("/Script/EnhancedInput.InputMappingContext'/Game/_Input/IMC_Kazan.IMC_Kazan'"));
	if (tempInputMapping.Succeeded())
	{
		DefaultMappingContext = tempInputMapping.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempMoveAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanMove.IA_KazanMove'"));
	if (tempMoveAction.Succeeded())
	{
		MoveAction = tempMoveAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempLookAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanLook.IA_KazanLook'"));
	if (tempLookAction.Succeeded())
	{
		LookAction = tempLookAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempAttackCAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanAttack_C.IA_KazanAttack_C'"));
	if (tempAttackCAction.Succeeded())
	{
		AttackCAction = tempAttackCAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempAttackSAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanAttack_S.IA_KazanAttack_S'"));
	if (tempAttackSAction.Succeeded())
	{
		AttackSAction = tempAttackSAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempParryAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanParry.IA_KazanParry'"));
	if (tempParryAction.Succeeded())
	{
		ParryAction = tempParryAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempEvadeAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_KazanEvade.IA_KazanEvade'"));
	if (tempEvadeAction.Succeeded())
	{
		EvadeAction = tempEvadeAction.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> tempLockOnAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_LockOn.IA_LockOn'"));
	if (tempLockOnAction.Succeeded())
	{
		LockOnAction = tempLockOnAction.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> tempSwitchTargetAction(
		TEXT("/Script/EnhancedInput.InputAction'/Game/_Input/IA_SwitchTarget.IA_SwitchTarget'"));
	if (tempSwitchTargetAction.Succeeded())
	{
		SwitchTargetAction = tempSwitchTargetAction.Object;
	}

	Kazan = nullptr;

	// UI 클래스 설정
	static ConstructorHelpers::FClassFinder<UPlayerStatusWidget> WidgetClassFinder(
		TEXT(
			"/Script/UMGEditor.WidgetBlueprint'/Game/_BluePrints/Widget/WB_PlayerStatusWidget.WB_PlayerStatusWidget_C'"));
	if (WidgetClassFinder.Succeeded())
	{
		StatusWidgetClass = WidgetClassFinder.Class;
	}
	
	ConstructorHelpers::FClassFinder<UUserWidget> missionFailWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/_BluePrints/Widget/WB_KazanHasFallen.WB_KazanHasFallen_C'"));
	if (missionFailWidget.Succeeded())
	{
		MissionFailClass = missionFailWidget.Class;
	}
}

void AKazanPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		this->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	Kazan = Cast<APlayer_Kazan>(GetPawn());

	// UI 초기화
	InitializeHUD();
}

void AKazanPlayerController::InitializeHUD()
{
	if (!StatusWidgetClass || !Kazan) return;

	// UI 위젯 생성
	StatusWidget = CreateWidget<UPlayerStatusWidget>(this, StatusWidgetClass);
	if (StatusWidget)
	{
		StatusWidget->AddToViewport();

		// HP 변경 이벤트 바인딩
		if (Kazan->m_HealthComponent)
		{
			Kazan->m_HealthComponent->OnHealthChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::UpdateHealth);
			// 초기값 설정
			StatusWidget->UpdateHealth(Kazan->GetHP(), 0.0f, Kazan->m_HealthComponent->GetMaxHP());
		}

		// Stamina 변경 이벤트 바인딩
		if (Kazan->m_StaminaComponent)
		{
			Kazan->m_StaminaComponent->OnStaminaChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::UpdateStamina);
			// 초기값 설정
			StatusWidget->UpdateStamina(Kazan->GetStamina(), 0.0f, Kazan->m_StaminaComponent->GetMaxStamina());
		}
	}

	FailWidget = CreateWidget<UUserWidget>(this, MissionFailClass);
}

void AKazanPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AKazanPlayerController::OnMove);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKazanPlayerController::OnLook);

		// Attack
		EnhancedInputComponent->BindAction(AttackCAction, ETriggerEvent::Started, this,
		                                   &AKazanPlayerController::On_Attack_Weak_Pressed);
		EnhancedInputComponent->BindAction(AttackSAction, ETriggerEvent::Started, this,
		                                   &AKazanPlayerController::On_Attack_Strong_Pressed);
		EnhancedInputComponent->BindAction(AttackSAction, ETriggerEvent::Completed, this,
		                                   &AKazanPlayerController::On_Attack_Strong_Released);
		EnhancedInputComponent->BindAction(AttackSAction, ETriggerEvent::Triggered, this,
		                                   &AKazanPlayerController::On_Attack_Strong_Triggered);

		// Parry
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this,
		                                   &AKazanPlayerController::On_Parry_Pressed);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Completed, this,
		                                   &AKazanPlayerController::On_Parry_Released);

		// Evade
		EnhancedInputComponent->BindAction(EvadeAction, ETriggerEvent::Started, this,
		                                   &AKazanPlayerController::On_Dodge_Pressed);

		// 락온 관련 입력 바인딩
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this,
		                                   &AKazanPlayerController::On_LockOn_Pressed);
		EnhancedInputComponent->BindAction(SwitchTargetAction, ETriggerEvent::Triggered, this,
		                                   &AKazanPlayerController::On_SwitchTarget_Triggered);
	}
	else
	{
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void AKazanPlayerController::OnMove(const FInputActionValue& Value)
{
	Kazan->Move(Value.Get<FVector2D>());
}

void AKazanPlayerController::OnLook(const FInputActionValue& Value)
{
	Kazan->Look(Value.Get<FVector2D>());
}

void AKazanPlayerController::On_Attack_Weak_Pressed(const FInputActionValue& InputActionValue)
{
	Kazan->Attack_Weak_Pressed();
}

void AKazanPlayerController::On_Attack_Strong_Pressed(const FInputActionValue& InputActionValue)
{
	Kazan->Attack_Strong_Pressed();
	//LOG_PRINT(TEXT("Press"));
}

void AKazanPlayerController::On_Attack_Strong_Released(const FInputActionValue& InputActionValue)
{
	Kazan->Attack_Strong_Released();
	//LOG_PRINT(TEXT("Release"));
}

void AKazanPlayerController::On_Attack_Strong_Triggered(const FInputActionValue& InputActionValue)
{
	//LOG_PRINT(TEXT("Trigger"));
}

void AKazanPlayerController::On_Parry_Pressed(const FInputActionValue& InputActionValue)
{
	Kazan->Guard_Pressed();
}

void AKazanPlayerController::On_Parry_Released(const FInputActionValue& InputActionValue)
{
	Kazan->Guard_Released();
}

void AKazanPlayerController::On_Dodge_Pressed(const FInputActionValue& InputActionValue)
{
	Kazan->Dodge_Pressed();
}

void AKazanPlayerController::On_Run_Pressed(const FInputActionValue& InputActionValue)
{
}

void AKazanPlayerController::On_Run_Released(const FInputActionValue& InputActionValue)
{
}

void AKazanPlayerController::AddCurrency(ECurrencyType CurrencyType, int CurrencyValue)
{
	if (CurrencyValue < 0) return;
	CurrencyValues[CurrencyType] += CurrencyValue;
	this->OnCurrencyChange.Broadcast(CurrencyType, CurrencyValues[CurrencyType], CurrencyValue);
}

void AKazanPlayerController::RemoveCurrency(ECurrencyType CurrencyType, int CurrencyValue)
{
	if (CurrencyValue < 0) return;
	// 스태미나 감소
	float oldCurrency = CurrencyValues[CurrencyType];
	CurrencyValues[CurrencyType] = FMath::Max(CurrencyValues[CurrencyType] - CurrencyValue, 0);
	this->OnCurrencyChange.Broadcast(CurrencyType, CurrencyValues[CurrencyType],
	                                 -(oldCurrency - CurrencyValues[CurrencyType]));
}

int AKazanPlayerController::GetCurrencyValue(ECurrencyType CurrencyType)
{
	return CurrencyValues[CurrencyType];
}

void AKazanPlayerController::On_LockOn_Pressed()
{
	if (!Kazan) return;

	LOG_PRINT(TEXT("OnLockOnPressed()"));
	if (ULockOnComponent* LockOnComp = Kazan->GetLockOnComponent())
	{
		// 록온 On / Off
		if (!LockOnComp->ToggleLockOn())
		{
			// 록온 On 상태에서 타겟이 없다면 플레이어 시선 방향으로 카메라 정렬 
			FRotator Rotator = Kazan->GetActorForwardVector().Rotation();
			Rotator.Pitch = -20.f;
			Kazan->RotateCameraWithSpeed(Rotator, 8.0f);
		}
	}
}

void AKazanPlayerController::On_SwitchTarget_Triggered(const FInputActionValue& Value)
{
	if (!Kazan) return;

	LOG_PRINT(TEXT("OnSwitchTarget()"));
	if (ULockOnComponent* LockOnComp = Kazan->GetLockOnComponent())
	{
		// 마우스 휠 값을 받아서 방향 결정
		float WheelDelta = Value.Get<float>();
		LOG_PRINT(TEXT("OnSwitchTarget() %f"), WheelDelta);

		// 휠 업/다운에 따라 좌우 전환
		FVector2D SwitchDirection;
		if (WheelDelta > 0)
		{
			SwitchDirection = FVector2D(1.0f, 0.0f); // 오른쪽으로 전환
		}
		else if (WheelDelta < 0)
		{
			SwitchDirection = FVector2D(-1.0f, 0.0f); // 왼쪽으로 전환
		}

		if (!SwitchDirection.IsZero())
		{
			LockOnComp->SwitchTarget(SwitchDirection);
		}
	}
}
