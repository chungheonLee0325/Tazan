// Fill out your copyright notice in the Description page of Project Settings.


#include "S_Attack.h"

#include <Tazan/Utilities/LogMacro.h>

#include "Algo/RandomShuffle.h"
#include "Kismet/KismetMathLibrary.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Skill/Base/BaseSkill.h"

void US_Attack::InitState()
{
}

void US_Attack::Enter()
{
	bIsAttack = false;
	LOG_SCREEN("공격");
	LOG_PRINT(TEXT("공격"));

	// 사용 가능한 스킬 목록
	TArray<int> arr = {1000, 1005, 1010};
	Algo::RandomShuffle(arr);

	FRotator rotator = UKismetMathLibrary::FindLookAtRotation(m_Owner->GetActorLocation(), m_Owner->GetAggroTarget()->GetActorLocation());
	m_Owner->SetActorRotation(rotator);

	UBaseSkill* Skill = m_Owner->NextSkill;
	if (m_Owner->CanCastSkill(Skill, m_Owner->GetAggroTarget()))
	{
		m_Owner->CastSkill(Skill, m_Owner->GetAggroTarget());
		m_Owner->GetCurrentSkill()->OnSkillComplete.BindUObject(this, &US_Attack::OnSkillComplete);
		bIsAttack = true;
		return;
	}

	//for (int idx : arr)
	//{
	//	Skill = m_Owner->GetSkillByID(idx);
	//	if (m_Owner->CanCastSkill(Skill,m_Owner->GetAggroTarget()))
	//	{
	//		m_Owner->CastSkill(Skill,m_Owner->GetAggroTarget());
	//		m_Owner->GetCurrentSkill()->OnSkillComplete.BindUObject(this ,&US_Attack::OnSkillComplete);
	//		bIsAttack = true;
	//		return;
	//	}
	//}

	//if (m_Owner->CanCastSkill(Skill,m_Owner->GetAggroTarget()))
	//{
	//	LOG_PRINT(TEXT("공격4"));
	//	m_Owner->CastSkill(Skill,m_Owner->GetAggroTarget());
	//	m_Owner->GetCurrentSkill()->OnSkillComplete.BindUObject(this ,&US_Attack::OnSkillComplete);
	//	bIsAttack = true;
	//}


	// if (RandValue < 3)
	// {
	// 	LOG_PRINT(TEXT("공격1"));
	// 	Skill = m_Owner->GetSkillByID(1000);
	// }
	// else if (RandValue < 6)
	// {
	// 	LOG_PRINT(TEXT("공격2"));
	// 	Skill = m_Owner->GetSkillByID(1005);
	// }
	// else
	// {
	// 	LOG_PRINT(TEXT("공격3"));
	// 	Skill = m_Owner->GetSkillByID(1010);
	// }
}

void US_Attack::Execute(float DeltaTime)
{
	if (bIsAttack == true)
	{
		// 기다림
	}
	else
	{
		// 스킬 자체가 실패한 경우
		m_AiFSM->ChangeState(EAiStateType::Wait);
	}
}

void US_Attack::Exit()
{
}

void US_Attack::OnSkillComplete()
{
	LOG_PRINT(TEXT("스킬 완료"));
	m_AiFSM->ChangeState(EAiStateType::Wait);
}
