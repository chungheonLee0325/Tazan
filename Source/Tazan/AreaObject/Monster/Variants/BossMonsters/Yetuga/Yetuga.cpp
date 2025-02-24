


#include "Yetuga.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/Animation/Monster/YetugaAnimInstance.h"
#include "Tazan/AreaObject/Attribute/StaminaComponent.h"
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

	// 예투가 넉백 거리 배율 설정
	KnockBackForceMultiplier = 0.0f;

	static ConstructorHelpers::FClassFinder<UPlayerStatusWidget> WidgetClassFinder(TEXT("/Script/UMG.WidgetBlueprintGeneratedClass'/Game/_BluePrints/Widget/WB_YetugaStatusWidget.WB_YetugaStatusWidget_C'"));
	if (WidgetClassFinder.Succeeded())
	{
		StatusWidgetClass = WidgetClassFinder.Class;
	}
	
	ConstructorHelpers::FClassFinder<UUserWidget> missionCompWidget(TEXT("/Script/UMG.WidgetBlueprintGeneratedClass'/Game/_BluePrints/Widget/WB_MissionComplete.WB_MissionComplete_C'"));
	if (missionCompWidget.Succeeded())
	{
		MissionCompleteClass = missionCompWidget.Class;
	}
}

void AYetuga::BeginPlay()
{
	Super::BeginPlay();
	
	m_AggroTarget = Cast<AAreaObject>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	YetugaABP = Cast<UYetugaAnimInstance>(GetMesh()->GetAnimInstance());
	InitializeHUD();
	SkillRoulette->InitSkill();

	//시작시 어퍼컷 콤보공격 확정 실행
	NextSkill = GetSkillByID(11000);
	m_AiFSM->ChangeState(EAiStateType::Chase);
}

UBaseAiFSM* AYetuga::CreateFSM()
{
	return CreateDefaultSubobject<UYetugaFSM>(TEXT("FSM"));
}

void AYetuga::ParryStackPenalty()
{
	Super::ParryStackPenalty();
	YetugaABP->CurrentAnimState = EYAnimState::ParryGroggyEnter;
}

void AYetuga::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AYetuga::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool AYetuga::IsWeakPointHit(const FVector& HitLoc)
{
	FVector hitDir = HitLoc - GetActorLocation();
	hitDir.Normalize();

	float dot = FVector::DotProduct(hitDir,GetActorForwardVector());
	if (dot < -0.2f)
	{
		// LOG_SCREEN("뒤에서 맞았다요.");
		return true;
	}
	return false;
}

void AYetuga::OnDie()
{
	Super::OnDie();
	CompleteWidget->AddToViewport();
}

void AYetuga::ChangeStateToWait()
{
	m_AiFSM->ChangeState(EAiStateType::Wait);
}

void AYetuga::ChangeStateToChase()
{
	m_AiFSM->ChangeState(EAiStateType::Chase);
}

void AYetuga::ChangeStateToAttack()
{
	m_AiFSM->ChangeState(EAiStateType::Attack);
}

void AYetuga::InitializeHUD()
{
	if (!StatusWidgetClass) return;
	if (!MissionCompleteClass) return;
	
	// UI 위젯 생성
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	StatusWidget = CreateWidget<UPlayerStatusWidget>(pc, StatusWidgetClass);
	CompleteWidget = CreateWidget<UUserWidget>(pc, MissionCompleteClass);
	
	if (StatusWidget)
	{
		StatusWidget->AddToViewport();
		
		// HP 변경 이벤트 바인딩
		if (m_HealthComponent)
		{
			m_HealthComponent->OnHealthChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::UpdateHealth);
			// 초기값 설정
			StatusWidget->UpdateHealth(GetHP(), 0.0f, m_HealthComponent->GetMaxHP());
		}
		
		// Stamina 변경 이벤트 바인딩
		if (m_StaminaComponent)
		{
			m_StaminaComponent->OnStaminaChanged.AddDynamic(StatusWidget, &UPlayerStatusWidget::UpdateStamina);
			// 초기값 설정
			StatusWidget->UpdateStamina(GetStamina(), 0.0f, m_StaminaComponent->GetMaxStamina());
		}
	}
}

