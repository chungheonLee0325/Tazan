// Fill out your copyright notice in the Description page of Project Settings.


#include "S_MoveTo.h"

#include <Tazan/Utilities/LogMacro.h>

#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"

void US_MoveTo::InitState()
{
	
}

void US_MoveTo::Enter()
{
	
}

void US_MoveTo::Execute(float DeltaTime)
{
	LOG_SCREEN("움직여");
	
	m_AiFSM->ChangeState(EAiStateType::Wait);
	
}

void US_MoveTo::Exit()
{
	
}
