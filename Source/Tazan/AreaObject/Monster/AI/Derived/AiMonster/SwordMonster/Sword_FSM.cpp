// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword_FSM.h"


#include "SwordAnim.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/Variants/NormalMonsters/SwordEnemy/SwordEnemy.h"
#include "Tazan/AreaObject/Monster/Ai/Derived/AiMonster/SwordMonster/S_Attack.h"
#include "Tazan/AreaObject/Monster/Ai/Derived/AiMonster/SwordMonster/S_Wait.h"
#include "Tazan/AreaObject/Monster/Ai/Derived/AiMonster/SwordMonster/S_MoveTo.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"


// Sets default values for this component's properties
USword_FSM::USword_FSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	
	
	
}


// Called when the game starts
void USword_FSM::BeginPlay()
{
	Super::BeginPlay();

	if (m_Owner->GetAggroTarget() == nullptr)
	{
		m_Owner->SetAggroTarget(Cast<APlayer_Kazan>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)));
	}
	if (Target == nullptr)
	{
		Target = Cast<APlayer_Kazan>(m_Owner->GetAggroTarget());
	}
	
	
}


// Called every frame
void USword_FSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USword_FSM::InitStatePool()
{
	
	ASwordEnemy* swordEnemy = Cast<ASwordEnemy>(m_Owner);
	if (!swordEnemy) return;

	//Move
	auto Moveto = CreateState<US_MoveTo>(this, m_Owner, EAiStateType::Idle);
	AddState(EAiStateType::Idle, Moveto);

	//wait
	auto Wait = CreateState<US_Wait>(this, m_Owner, EAiStateType::Wait);
	AddState(EAiStateType::Wait, Wait);
	
	//Attack
	auto Attack = CreateState<US_Attack>(this, m_Owner, EAiStateType::Attack);
	AddState(EAiStateType::Attack, Attack);
	
	ChangeState(EAiStateType::Wait);


	// UFUNCTION()
	// bool IsPlayerInCheckRadius();
	//
	// UFUNCTION()
	// bool IsPlayerInAttackRadius();
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "S_FSM")
	// float CheckRadius;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "S_FSM")
	// float AttackRadius;
	
}

