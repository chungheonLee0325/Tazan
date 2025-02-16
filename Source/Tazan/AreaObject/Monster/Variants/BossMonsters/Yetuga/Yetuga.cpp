


#include "Yetuga.h"

#include "GameFramework/CharacterMovementComponent.h"
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
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;

	m_AreaObjectID = 100;
}

void AYetuga::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<APlayer_Kazan>(GetWorld()->GetFirstPlayerController()->GetPawn());
	SkillRoulette->InitSkill();

	m_CurrentSkill = GetSkillByID(11000);
	m_AiFSM->ChangeState(EAiStateType::SelectSkill);
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

float AYetuga::DistToPlayer()
{
	return GetDistanceTo(Player);
}

float AYetuga::GetPlayerDir()
{
	// GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorForwardVector();
	FVector dir = Player->GetActorLocation() - GetActorLocation();
	dir.Normalize();
	return FVector::DotProduct(dir,GetActorForwardVector());
}

void AYetuga::ShortAttack()
{
}

void AYetuga::LongAtk()
{
}
