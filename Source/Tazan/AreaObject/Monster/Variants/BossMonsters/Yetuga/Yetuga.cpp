


#include "Yetuga.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/Yetuga/YetugaFSM.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
#include "Tazan/AreaObject/Skill/Monster/BossMonsters/Yetuga/Y_SkillRoulette.h"


AYetuga::AYetuga()
{
	PrimaryActorTick.bCanEverTick = true;
	m_AiFSM = AYetuga::CreateFSM();

	SkillRoulette = CreateDefaultSubobject<UY_SkillRoulette>(TEXT("SkillRoulette"));

	ConstructorHelpers::FObjectFinder<UAnimMontage>move(TEXT("/Game/_Resource/Yetuga/Animation/AM_SideMoveAtk_L.AM_SideMoveAtk_L"));
	if (move.Object)
	{
	}
	GetCharacterMovement()->MaxWalkSpeed = 1200.0f;

	m_AreaObjectID = 100;
}

void AYetuga::BeginPlay()
{
	Super::BeginPlay();
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

