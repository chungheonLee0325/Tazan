


#include "Yetuga.h"

#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/Yetuga/YetugaFSM.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"


AYetuga::AYetuga()
{
	PrimaryActorTick.bCanEverTick = true;
	m_AiFSM = AYetuga::CreateFSM();
}

void AYetuga::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<APlayer_Kazan>(GetWorld()->GetFirstPlayerController());
	GetMesh()->GetAnimInstance()->Montage_Play(amMovingAtk);
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

void AYetuga::WeaveingAttack(uint8 idx)
{
	UE_LOG(LogTemp, Display, TEXT("%d"),idx);
	GetMesh()->GetAnimInstance()->Montage_Play(amMovingAtk);
	
	if (idx == 0)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(amMovingAtk);
	}
	else if (idx == 1)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(amSweapAtk);
	}
	else if (idx == 2)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(amThrowRockAtk);
	}
	else if (idx == 3)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(amRoar);
	}
}

void AYetuga::ShortAttack()
{
}

void AYetuga::LongAtk()
{
}
