// Fill out your copyright notice in the Description page of Project Settings.


#include "Yetuga_RockS.h"

#include "Components/SphereComponent.h"


AYetuga_RockS::AYetuga_RockS()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AYetuga_RockS::BeginPlay()
{
	Super::BeginPlay();
	
}

void AYetuga_RockS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

