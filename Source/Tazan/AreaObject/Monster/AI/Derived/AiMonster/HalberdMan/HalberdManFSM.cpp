// Fill out your copyright notice in the Description page of Project Settings.


#include "HalberdManFSM.h"


// Sets default values for this component's properties
UHalberdManFSM::UHalberdManFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHalberdManFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHalberdManFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHalberdManFSM::InitStatePool()
{
	//AYetuga* yetuga = Cast<AYetuga>(m_Owner);
	//if (!yetuga) return;
	//
	//// Wait 
	//auto StandOff = CreateState<UY_StandOff>(this, m_Owner, EAiStateType::Wait);
	//AddState(EAiStateType::Wait, StandOff);
	//// StandOff->SetNextState(EAiStateType::SelectSkill);
	//
	//// Idle
	//auto Groggy = CreateState<UY_Groggy>(this, m_Owner, EAiStateType::Idle);
	//AddState(EAiStateType::Idle, Groggy);
	//
	//// Attack
	//auto Attack = CreateState<UY_Attack>(this, m_Owner, EAiStateType::Attack);
	//AddState(EAiStateType::Attack, Attack);
	//Attack->SetNextState(EAiStateType::SelectSkill);
	//
	////SelectSkill
	//auto SelectSkill = CreateState<UY_SelectSkill>(this, m_Owner, EAiStateType::SelectSkill);
	//SelectSkill->SetSkillRoulette(yetuga->SkillRoulette);
	//AddState(EAiStateType::SelectSkill, SelectSkill);
	//
	//// Chase
	//auto Chase = CreateState<UY_Chase>(this, m_Owner, EAiStateType::Chase);
	//AddState(EAiStateType::Chase, Chase);
	//Chase->SetNextState(EAiStateType::Attack);
}

