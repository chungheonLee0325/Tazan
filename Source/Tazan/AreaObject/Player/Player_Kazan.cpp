// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Kazan.h"


// Sets default values
APlayer_Kazan::APlayer_Kazan()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayer_Kazan::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayer_Kazan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayer_Kazan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

