


#include "Yetuga.h"

#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/Yetuga/YetugaFSM.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"


AYetuga::AYetuga()
{
	PrimaryActorTick.bCanEverTick = true;
	m_AiFSM = AYetuga::CreateFSM();
}

float AYetuga::DistToPlayer()
{
	return GetDistanceTo(Player);
}


void AYetuga::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<APlayer_Kazan>(GetWorld()->GetFirstPlayerController()->GetPawn());
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

UAnimMontage* AYetuga::GetAnimMontage(EYetugaAnimType animType)
{
	if (const TObjectPtr<UAnimMontage>* MontagePtr = AnimMontageMap.Find(animType))
	{
		return *MontagePtr;
	}
	return nullptr;
}

bool AYetuga::IsPlayerForward()
{
	// GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorForwardVector();
	FVector dir = Player->GetActorLocation() - GetActorLocation();
	dir.Normalize();
	float dot = FVector::DotProduct(dir,GetActorForwardVector());

	LOG_SCREEN("플레이어 정면, %f", dot);
	if (dot > 0.4)
	{
		return true;
	}
	return false;
}

void AYetuga::ShortAttack()
{
}

void AYetuga::LongAtk()
{
}
