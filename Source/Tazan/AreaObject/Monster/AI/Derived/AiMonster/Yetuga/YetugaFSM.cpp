﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "YetugaFSM.h"

#include "Y_Chase.h"
#include "Y_Groggy.h"
#include "Y_SelectSkill.h"
#include "Y_StandOff.h"
#include "Y_Attack.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"


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
	StandOff->SetNextState(EAiStateType::SelectSkill);
	StandOff->SetSkillRoulette(yetuga->SkillRoulette);
	
	// Idle
	auto Groggy = CreateState<UY_Groggy>(this, m_Owner, EAiStateType::Idle);
	AddState(EAiStateType::Idle, Groggy);

	// Attack
	auto Attack = CreateState<UY_Attack>(this, m_Owner, EAiStateType::Attack);
	AddState(EAiStateType::Attack, Attack);
	Attack->SetNextState(EAiStateType::Wait);

	//SelectSkill
	auto SelectSkill = CreateState<UY_SelectSkill>(this, m_Owner, EAiStateType::SelectSkill);
	SelectSkill->SetSkillRoulette(yetuga->SkillRoulette);
	AddState(EAiStateType::SelectSkill, SelectSkill);

	// Chase
	auto Chase = CreateState<UY_Chase>(this, m_Owner, EAiStateType::Chase);
	AddState(EAiStateType::Chase, Chase);
	Chase->SetNextState(EAiStateType::Attack);
}

