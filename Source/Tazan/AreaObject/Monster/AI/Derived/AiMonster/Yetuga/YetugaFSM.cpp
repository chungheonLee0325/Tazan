﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "YetugaFSM.h"

#include "Y_Groggy.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/Yetuga/Y_StandOff.h"
#include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/Yetuga/Y_Attack.h"
// #include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/Yetuga/Y_Groggy.h"
// #include "Tazan/AreaObject/Monster/AI/Derived/AiMonster/Yetuga/Y_Chase.h"


UYetugaFSM::UYetugaFSM()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UYetugaFSM::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UYetugaFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UYetugaFSM::InitStatePool()
{
	AYetuga* yetuga = Cast<AYetuga>(m_Owner);
	if (!yetuga) return;

	// Wait 
	auto StandOff = CreateState<UY_StandOff>(this, m_Owner, EAiStateType::Wait);
	AddState(EAiStateType::Wait, StandOff);
	ChangeState(EAiStateType::Attack);
	
	// Idle
	auto Groggy = CreateState<UY_Groggy>(this, m_Owner, EAiStateType::Idle);
	AddState(EAiStateType::Idle, Groggy);

	// Attack
	auto Attack = CreateState<UY_Attack>(this, m_Owner, EAiStateType::Attack);
	AddState(EAiStateType::Attack, Attack);

	// // Chase
	// auto Chase = CreateState<UY_Chase>(this, m_Owner, EAiStateType::Chase);
	// AddState(EAiStateType::Chase, Chase);

	ChangeState(EAiStateType::Attack);
}

