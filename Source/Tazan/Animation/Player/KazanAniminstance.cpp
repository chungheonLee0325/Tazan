// Fill out your copyright notice in the Description page of Project Settings.


#include "KazanAniminstance.h"

#include "Tazan/AreaObject/Player/Player_Kazan.h"

void UKazanAniminstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 소유하고 있는 폰 얻어오기
	auto ownerPawn = TryGetPawnOwner();
	// TPSPlayer 로 cast 해야 한다.
	APlayer_Kazan * player = Cast<APlayer_Kazan>(ownerPawn);
	if (player)
	{
		// walkSpeed 값을 설정
		FVector velocity = player->GetVelocity();
		// 전방벡터
		FVector forward = player->GetActorForwardVector();
		speed = FVector::DotProduct(velocity, forward);

		// 좌우방향
		FVector right = player->GetActorRightVector();
		direction = FVector::DotProduct(velocity, right);
	}
}
