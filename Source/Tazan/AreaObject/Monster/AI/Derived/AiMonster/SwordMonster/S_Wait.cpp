// Fill out your copyright notice in the Description page of Project Settings.


#include "S_Wait.h"
#include <Tazan/Utilities/LogMacro.h>
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"

void US_Wait::InitState()
{
	
}

void US_Wait::Enter()
{
	LOG_SCREEN("기다려");
	
}

void US_Wait::Execute(float DeltaTime)
{
	
	// 시간이 흐른다 
	CurrentTime += DeltaTime;

	// 흐른 시간이 딜레이 타임보다 커지면
	if (CurrentTime > WaitDelayTime)
	{
		// 이동 상태로 전환 한다
		LOG_SCREEN("이동할래 타겟어딧어 "); 
		m_AiFSM->ChangeState(EAiStateType::Idle);
		//그리고 값을 초기화 한다 
		CurrentTime = 0;
		

	}

}

void US_Wait::Exit()
{
	
}



	



