// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordAnim.h"

void USwordAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// SwordEnemy를 매번 잘 관찰해서
	// 지금이 Idle인지, Move인지, Attack인지 알고싶다.
	/*if (지금이 Idle이다)
	{
		SwordState = ESwordState::Idle;
	}
	else if (ㄴㄴ 지금은 Move임)
	{
		SwordState = ESwordState::Move;
	}
	else if (ㄴㄴ 지금은 Attack임)
	{
		SwordState = ESwordState::Attack;
	}*/

}
