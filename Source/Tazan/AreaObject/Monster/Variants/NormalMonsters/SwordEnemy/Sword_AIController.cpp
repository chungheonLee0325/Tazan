// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword_AIController.h"


// Sets default values
ASword_AIController::ASword_AIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASword_AIController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASword_AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
} 

