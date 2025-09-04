


#include "Yetuga.h"

#include "Yetuga_RockS.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/Animation/Monster/YetugaAnimInstance.h"
#include "Tazan/AreaObject/Attribute/StaminaComponent.h"
#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/Yetuga/YetugaFSM.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"

#include "Tazan/UI/Widget/PlayerStatusWidget.h"


AYetuga::AYetuga()
{
	PrimaryActorTick.bCanEverTick = true;
	
	m_AiFSM = AYetuga::CreateFSM();
	m_SkillRoulette = AYetuga::CreateSkillRoulette();
	
	GetCharacterMovement()->MaxWalkSpeed = 1200.0f;
	
	m_AreaObjectID = 100;
	
	m_KnockBackForceMultiplier = 0.0f;
	
	bIsParrySkill = false;

	SmallRockMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SmallRockMesh"));
	SmallRockMesh->SetupAttachment(GetMesh(),FName("weapon_r_Socket"));
	SmallRockMesh->SetWorldScale3D(FVector(0.15f, 0.15f, 0.15f));
	SmallRockMesh->SetVisibility(false);
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh>rockMesh(TEXT("/Game/_Resource/Yetuga/Rock/Yetuga_SmallRock.Yetuga_SmallRock"));
	if (rockMesh.Succeeded())
	{
		SmallRockMesh->SetSkeletalMesh(rockMesh.Object);
	}

	HPWidgetComponent->SetVisibility(false);

	ConstructorHelpers::FClassFinder<UPlayerStatusWidget> YetugaHPWidget(TEXT("/Script/UMG.WidgetBlueprintGeneratedClass'/Game/_BluePrints/Widget/WB_YetugaStatusWidget.WB_YetugaStatusWidget_C'"));
	if (YetugaHPWidget.Succeeded())
	{
		BossStatusWidgetClass = YetugaHPWidget.Class;
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
}

UBaseAiFSM* AYetuga::CreateFSM()
{
	return CreateDefaultSubobject<UYetugaFSM>(TEXT("FSM"));
}

UBaseSkillRoulette* AYetuga::CreateSkillRoulette()
{
	return CreateDefaultSubobject<UY_SkillRoulette>(TEXT("SkillRouletteComponent"));
}

void AYetuga::ParryStackPenalty()
{
	// if (m_CurrentSkill)
	// {
	// 	m_CurrentSkill->CancelCast();
	// 	ClearCurrentSkill();
	// }

	bIsParrySkill = false;
	InitParryStack();
	
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst)
	{
		if (ParryPenaltyAnimation != nullptr)
		{
			animInst->Montage_Play(ParryPenaltyAnimation);
			ChangeStateToGroggy();
		}
		else
		{
			LOG_SCREEN_ERROR(this, "패리 패널티 애니 비어있음");
		}
	}
	
	YetugaABP->CurrentAnimState = EYAnimState::ParryGroggyEnter;
}

void AYetuga::HandleGroggy(float Duration)
{
	Super::HandleGroggy(Duration);
	
	m_AiFSM->ChangeState(EAiStateType::Idle);
	
	YetugaABP->CurrentAnimState = EYAnimState::NormalGroggyEnter;
	
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst)
	{
		animInst->Montage_Stop(0.25f);
	}
}

void AYetuga::OnGroggyEnd()
{
	YetugaABP->CurrentAnimState = EYAnimState::GroggyEnd;
	Recover();
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

void AYetuga::YetugaStart()
{
	InitializeHUD();
	
	//시작시 어퍼컷 콤보공격 확정 실행
	NextSkill = GetSkillByID(StartSkillID);
	m_AiFSM->ChangeState(EAiStateType::Chase);
}

void AYetuga::OnStun(UAnimMontage* AnimMontage, bool bArg)
{
	YetugaABP->CurrentAnimState = EYAnimState::GroggyProcess;
}

void AYetuga::ChargeSkillStun()
{
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst)
	{
		if (ChargeStunAni != nullptr)
		{
			// 델리게이트 객체 생성
			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &AYetuga::OnStun);
        
			// 몽타주가 끝날 때 위 델리게이트가 호출되도록 바인딩
			animInst->Montage_SetEndDelegate(MontageEndedDelegate, ChargeStunAni);
			
			animInst->Montage_Play(ChargeStunAni);
			ChangeStateToGroggy();
		}
		else
		{
			LOG_SCREEN_ERROR(this,"차지 스턴 애니 비어있음");
		}
	}
}

void AYetuga::Recover()
{
	YetugaABP->CurrentAnimState = EYAnimState::None;
	
	NextSkill = GetSkillByID(19000);
	m_AiFSM->ChangeState(EAiStateType::Attack);
}

void AYetuga::FastBall()
{
	AYetuga_RockS* spawnedRock = GetWorld()->SpawnActor<AYetuga_RockS>(AYetuga_RockS::StaticClass(),SmallRockMesh->GetComponentLocation(),SmallRockMesh->GetComponentRotation());
	
	if (spawnedRock)
	{
		// ToDo : Init으로 묶기 + Init에서 FAttackData 넘겨주기
		//FAttackData* AttackData = m_CurrentSkill->GetAttackDataByIndex(index);
		spawnedRock->InitRock(this,m_AggroTarget);
		spawnedRock->Fire();
	}
}



void AYetuga::ShowRock()
{
	SmallRockMesh->SetVisibility(true);
}

void AYetuga::HideRock()
{
	SmallRockMesh->SetVisibility(false);
}

void AYetuga::StartAnimMove()
{
	USkeletalMeshComponent* SK = GetMesh();
	if (!SK)
	{
		UE_LOG(LogTemp, Error, TEXT("스켈레탈 메시 NULL"));
		return;
	}

	// 애니메이션 시작 시 pelvis의 월드 위치 저장
	StartPelvisWS = SK->GetSocketLocation("pelvis");
	LOG_SCREEN("시작 위치:, X: %f, Y: %f, Z: %f ",StartPelvisWS.X,StartPelvisWS.Y,StartPelvisWS.Z);
	UE_LOG(LogTemp,Warning,TEXT("pelvis 시작 위치:, X: %f, Y: %f, Z: %f "),StartPelvisWS.X,StartPelvisWS.Y,StartPelvisWS.Z);
	UE_LOG(LogTemp,Error,TEXT("액터 시작 위치:, X: %f, Y: %f, Z: %f "),GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z);

	bIsAnimMoving = true;
}

void AYetuga::EndAnimMove()
{
	USkeletalMeshComponent* SK = GetMesh();
	if (!SK)
	{
		UE_LOG(LogTemp, Error, TEXT("스켈레탈 메시 NULL"));
		return;
	}

	FVector EndPelvisWS = SK->GetSocketLocation("pelvis");
	FVector Delta = EndPelvisWS - StartPelvisWS;

	float scale = 2.0f;
	AddActorWorldOffset(FVector(Delta.X * scale,Delta.Y * scale,0), true);
	LOG_SCREEN("종료 위치:, X: %f, Y: %f, Z: %f ",EndPelvisWS.X,EndPelvisWS.Y,EndPelvisWS.Z);
	UE_LOG(LogTemp,Warning,TEXT("pelvis 종료 위치:, X: %f, Y: %f, Z: %f "),EndPelvisWS.X,EndPelvisWS.Y,EndPelvisWS.Z);
	UE_LOG(LogTemp,Warning,TEXT("보정 값:, X: %f, Y: %f, Z: %f "),Delta.X,Delta.Y,0.0f);
	UE_LOG(LogTemp,Error,TEXT("액터 종료 위치:, X: %f, Y: %f, Z: %f "),GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z);

	bIsAnimMoving = false;
}

void AYetuga::AnimMove()
{
	USkeletalMeshComponent* SK = GetMesh();
	if (!bIsAnimMoving || !SK) return;

	// pelvis의 현재 '컴포넌트 공간' 위치
	FVector CurrentPelvisLS = SK->GetSocketTransform("pelvis", RTS_Component).GetLocation();
	FVector DeltaLS = CurrentPelvisLS - PrevPelvisWS;

	// 컴포넌트 방향에 맞춰 월드 공간 방향으로 변환
	FVector DeltaWS = SK->GetComponentTransform().TransformVectorNoScale(DeltaLS);

	AddActorWorldOffset(DeltaWS, true);

	PrevPelvisWS = CurrentPelvisLS;
}

void AYetuga::InitializeHUD()
{
	if (!BossStatusWidgetClass) return;
	if (!MissionCompleteClass) return;
	
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	
	StatusWidget = CreateWidget<UPlayerStatusWidget>(pc, BossStatusWidgetClass);
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

