// Fill out your copyright notice in the Description page of Project Settings.


#include "AiBurrbo.h"

#include "Tazan/AreaObject/Monster/AI/Derived/CommonState/AggroWait.h"
#include "Tazan/AreaObject/Monster/AI/Derived/CommonState/CommonAttack.h"
#include "Tazan/AreaObject/Monster/Variants/NormalMonsters/Burrbo/Burrbo.h"


class UCommonAttack;
// Sets default values for this component's properties
UAiBurrbo::UAiBurrbo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAiBurrbo::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAiBurrbo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAiBurrbo::InitStatePool()
{
	ABurrbo* Burrbo = Cast<ABurrbo>(m_Owner);
	if (!Burrbo) return;
	
	m_OriginalPosition = m_Owner->GetActorLocation();

	// AggroWait 상태 설정
	auto AggroWait = CreateState<UAggroWait>(this, m_Owner, EAiStateType::Idle);
	AggroWait->SetM_DetectRange(Burrbo->GetDetectRange());
	AggroWait->SetM_WaitTime(0.3f);
	AggroWait->SetNextState(EAiStateType::Attack);
	AddState(EAiStateType::Idle, AggroWait);

	// Attack 상태 설정
	auto Attack = CreateState<UCommonAttack>(this, m_Owner, EAiStateType::Attack);
	Attack->SetNextState(EAiStateType::Idle);
	AddState(EAiStateType::Attack, Attack);

	ChangeState(EAiStateType::Idle);;
}

