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
	Yetuga = Cast<AYetuga>(m_Owner);
	m_NextState = EAiStateType::Wait;

	//랜덤으로 스킬 선택
	UTazanGameInstance* gi = Cast<UTazanGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (gi)
	{
		gi->GetDataSkill(100);
	}
	AnimMontagePlay(Yetuga,Yetuga->GetAnimMontage(EYetugaAnimType::NormalAtk));
}

void UY_Attack::Execute(float dt)
{
}

void UY_Attack::Exit()
{
}

void UY_Attack::AnimMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	Super::AnimMontageEnd(Montage, bInterrupted);
}

int UY_Attack::GetRandomSkill()
{
	return 100;
}

int UY_Attack::GetRandomSkill(int startID, int endID)
{
	return 100;
}


