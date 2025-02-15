// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_Attack.h"

#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/Contents/TazanGameInstance.h"

void UY_Attack::InitState()
{
}

void UY_Attack::Enter()
{
	LOG_SCREEN("어택");
	AYetuga* yetuga = Cast<AYetuga>(m_Owner);
	m_NextState = EAiStateType::Wait;

	//TODO: SkillBag?에서 nextSkill 값가져와서 스킬 실행
	AAreaObject* areaObj = Cast<AAreaObject>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	m_Owner->CastSkill(m_Owner->GetCurrentSkill(),areaObj);
	
	//TODO: 몽타주 플레이 레거시
	// AnimMontagePlay(yetuga,yetuga->GetAnimMontage(EWeavingSkillAnim::UpperCut));
}

void UY_Attack::Execute(float dt)
{
}

void UY_Attack::Exit()
{
}


