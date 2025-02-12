


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

void AYetuga::PlayAnimMontage(EYetugaAnimType animType)
{
	if (UAnimMontage* Montage = GetAnimMontage(animType))
	{
		if (GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(Montage);
		}
	}
}

UAnimMontage* AYetuga::GetAnimMontage(EYetugaAnimType animType)
{
	if (const TObjectPtr<UAnimMontage>* MontagePtr = AnimMontageMap.Find(animType))
	{
		return *MontagePtr;
	}
	return nullptr;
}


void AYetuga::ShortAttack()
{
}

void AYetuga::LongAtk()
{
}
