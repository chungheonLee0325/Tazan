// Fill out your copyright notice in the Description page of Project Settings.


#include "testEnemy.h"

#include "Components/CapsuleComponent.h"
#include "Runtime/AIModule/Classes/AIController.h"


// Sets default values
AtestEnemy::AtestEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

// Called when the game starts or when spawned
void AtestEnemy::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AtestEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AtestEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

