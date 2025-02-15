// Fill out your copyright notice in the Description page of Project Settings.


#include "S_MoveTo.h"
#include <Tazan/Utilities/LogMacro.h>
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Monster/AI/Base/BaseAiFSM.h"
#include "Tazan/AreaObject/Monster/Variants/NormalMonsters/SwordEnemy/SwordEnemy.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"

// Overview : 플레이어를 추적하고 싶다


void US_MoveTo::InitState()
{
}

void US_MoveTo::Enter()
{
	// Todo : YT@@ 플레이어가 누군지 찾아보자  
	if (Target == nullptr)
	{
		auto player = UGameplayStatics::GetPlayerPawn(this, 0);
		Target = Cast<APlayer_Kazan>(player);
		LOG_SCREEN("찾음");
	}


	

}
	// Todo : YT@@ 내 반경을 설정해
	// Todo : YT@@ 설정 반경 안에 플레이어가있는지 체크해
	// Todo : YT@@ 만약 있다면  공격하러 갈꺼야
	// Todo : YT@@ 그렇지 않다면 다시 플레이어 방향으로 이동해 
void US_MoveTo::Execute(float DeltaTime)
{
	//  나중에  dv - 매틱마다 거리를 재야할까? ?? 1초에 한번은 안되나?


	//플레이어한테가는 이동방향구하기 
	FVector direction = Target->GetActorLocation() - m_Owner->GetActorLocation();
	direction.Normalize();
	//반경을 설정했어 
	float distance = direction.Size();

	// 이동 로직 
	m_Owner->AddMovementInput(direction);
	LOG_SCREEN("움직여");
	
	// 타겟과의거리가 공격범위보다 작으면 
	if (distance < AttackRange)
	{
		//공격하러간다
		m_AiFSM->ChangeState(EAiStateType::Attack);
		LOG_SCREEN("공격하러갈래");
		
	}
	
	m_Owner->AddMovementInput(direction);
	LOG_SCREEN("움직여");
	
	
}

void US_MoveTo::Exit()
{
}
