// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordAnim.h"

#include "Tazan/AreaObject/Monster/Variants/NormalMonsters/SwordEnemy/SwordEnemy.h"

void USwordAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto ownerPawn = TryGetPawnOwner();
	auto Enemy = Cast<ASwordEnemy>(ownerPawn);
	if (ownerPawn)
	{
		// walkSpeed 값을 설정
		FVector velocity = Enemy->GetVelocity();
		// 전방벡터
		FVector forward = Enemy->GetActorForwardVector();
		Speed = FVector::DotProduct(velocity, forward);

		// 좌우방향
		FVector right = Enemy->GetActorRightVector();
		direction = FVector::DotProduct(velocity, right);
		
	}
}
