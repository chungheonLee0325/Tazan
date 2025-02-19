


#include "Yetuga.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Attribute/Stamina.h"
#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/Yetuga/YetugaFSM.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_SkillRoulette.h"
#include "Tazan/UI/Widget/PlayerStatusWidget.h"


AYetuga::AYetuga()
{
	PrimaryActorTick.bCanEverTick = true;
	m_AiFSM = AYetuga::CreateFSM();

	SkillRoulette = CreateDefaultSubobject<UY_SkillRoulette>(TEXT("SkillRoulette"));
	
	GetCharacterMovement()->MaxWalkSpeed = 1200.0f;

	m_AreaObjectID = 100;

	static ConstructorHelpers::FClassFinder<UPlayerStatusWidget> WidgetClassFinder(TEXT("/Script/UMG.WidgetBlueprintGeneratedClass'/Game/_BluePrints/Widget/WB_YetugaStatusWidget.WB_YetugaStatusWidget_C'"));
	if (WidgetClassFinder.Succeeded())
	{
		StatusWidgetClass = WidgetClassFinder.Class;
	}
}

void AYetuga::BeginPlay()
{
	Super::BeginPlay();
	InitializeHUD();
	
	m_AggroTarget = Cast<AAreaObject>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	SkillRoulette->InitSkill();

	//시작시 어퍼컷 콤보공격 확정 실행
	NextSkill = GetSkillByID(11000);
	m_AiFSM->ChangeState(EAiStateType::Chase);
}

UBaseAiFSM* AYetuga::CreateFSM()
{
	return CreateDefaultSubobject<UYetugaFSM>(TEXT("FSM"));
}

void AYetuga::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AYetuga::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAnimMontage* AYetuga::GetAnimMontage(EWeavingSkillAnim animType)
{
	if (const TObjectPtr<UAnimMontage>* MontagePtr = AnimMontageMap.Find(animType))
	{
		return *MontagePtr;
	}
	return nullptr;
}

void AYetuga::InitializeHUD()
{
	if (!StatusWidgetClass) return;
	
	// UI 위젯 생성
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	StatusWidget = CreateWidget<UPlayerStatusWidget>(pc, StatusWidgetClass);
	if (StatusWidget)
	{
		StatusWidget->AddToViewport();
		
		// HP 변경 이벤트 바인딩
		if (m_Health)
		{
			m_Health->OnHealthChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::UpdateHealth);
			// 초기값 설정
			StatusWidget->UpdateHealth(GetHP(), 0.0f, m_Health->GetMaxHP());
		}
		
		// Stamina 변경 이벤트 바인딩
		if (m_Stamina)
		{
			m_Stamina->OnStaminaChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::UpdateStamina);
			// 초기값 설정
			StatusWidget->UpdateStamina(GetStamina(), 0.0f, m_Stamina->GetMaxStamina());
		}
	}
}

