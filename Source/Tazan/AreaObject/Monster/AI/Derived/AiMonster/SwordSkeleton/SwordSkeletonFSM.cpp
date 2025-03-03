// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordSkeletonFSM.h"


USwordSkeletonFSM::USwordSkeletonFSM()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USwordSkeletonFSM::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USwordSkeletonFSM::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USwordSkeletonFSM::InitStatePool()
{
	Super::InitStatePool();
}
