// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordEnemy.h"


// Sets default values
ASwordEnemy::ASwordEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/_Resource/SkeletonSword/C_M_SkeletonSword.C_M_SkeletonSword'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
}

// Called when the game starts or when spawned
void ASwordEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASwordEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASwordEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASwordEnemy::ResizeCharacterMesh(FVector NewScale)
{
	if (GetMesh())
	{
		// 스케일을 정확히 0.5로 설정
		GetMesh()->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	}
};