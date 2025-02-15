// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_SelectSkill.h"

#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"

void UY_SelectSkill::InitState()
{
}

void UY_SelectSkill::Enter()
{
	LOG_SCREEN("스킬 선택");
	//TODO: SkillBag에서 쿨타임이 아닌 스킬 중 랜덤으로 선택
	//TODO: SkillBag?은 선택된 스킬 nextSkill 등 변수에 저장해두기
	//스킬의 사정거리 값을 가져와 Chase상태로 변경 or Attack상태로 변경
	float skillRange = 300.0f;
	float dist = m_Owner->GetDistanceTo(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (dist > skillRange)
	{
		m_AiFSM->ChangeState(EAiStateType::Chase);
		return;
	}
	m_AiFSM->ChangeState(EAiStateType::Attack);
}

void UY_SelectSkill::Execute(float DeltaTime)
{
}

void UY_SelectSkill::Exit()
{
}
