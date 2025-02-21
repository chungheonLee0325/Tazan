// Fill out your copyright notice in the Description page of Project Settings.


#include "KazanPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player_Kazan.h"
#include "Tazan/AreaObject/Attribute/Stamina.h"
#include "Tazan/UI/Widget/PlayerStatusWidget.h"

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

	Kazan = nullptr;

	// UI 클래스 설정
	static ConstructorHelpers::FClassFinder<UPlayerStatusWidget> WidgetClassFinder(
		TEXT(
			"/Script/UMGEditor.WidgetBlueprint'/Game/_BluePrints/Widget/WB_PlayerStatusWidget.WB_PlayerStatusWidget_C'"));
	if (WidgetClassFinder.Succeeded())
	{
		StatusWidgetClass = WidgetClassFinder.Class;
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
		if (Kazan->m_Health)
		{
			Kazan->m_Health->OnHealthChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::UpdateHealth);
			// 초기값 설정
			StatusWidget->UpdateHealth(Kazan->GetHP(), 0.0f, Kazan->m_Health->GetMaxHP());
		}

		// Stamina 변경 이벤트 바인딩
		if (Kazan->m_Stamina)
		{
			Kazan->m_Stamina->OnStaminaChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::UpdateStamina);
			// 초기값 설정
			StatusWidget->UpdateStamina(Kazan->GetStamina(), 0.0f, Kazan->m_Stamina->GetMaxStamina());
		}
	}
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

		// Parry
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this,
		                                   &AKazanPlayerController::On_Parry_Pressed);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Completed, this,
		                                   &AKazanPlayerController::On_Parry_Released);

		// Evade
		EnhancedInputComponent->BindAction(EvadeAction, ETriggerEvent::Started, this,
		                                   &AKazanPlayerController::On_Dodge_Pressed);
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
	this->OnCurrencyChange.Broadcast(CurrencyType,CurrencyValues[CurrencyType],CurrencyValue);
}

void AKazanPlayerController::RemoveCurrency(ECurrencyType CurrencyType, int CurrencyValue)
{
	if (CurrencyValue < 0) return;
	// 스태미나 감소
	float oldCurrency = CurrencyValues[CurrencyType];
	CurrencyValues[CurrencyType] = FMath::Max(CurrencyValues[CurrencyType] - CurrencyValue, 0);
	this->OnCurrencyChange.Broadcast(CurrencyType,CurrencyValues[CurrencyType],-(oldCurrency - CurrencyValues[CurrencyType]));
}

int AKazanPlayerController::GetCurrencyValue(ECurrencyType CurrencyType)
{
	return CurrencyValues[CurrencyType];
}
