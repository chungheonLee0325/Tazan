// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostTrail.h"
#include "Components/PoseableMeshComponent.h"


AGhostTrail::AGhostTrail()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // 초기에는 틱 비활성화

	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("POSEABLEMESH"));
	RootComponent = PoseableMesh;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> tempGhostTail(
		TEXT("/Script/Engine.Material'/Game/_Resource/Materials/GhostMaterial.GhostMaterial'"));
	if (tempGhostTail.Succeeded())
	{
		GhostMaterial = tempGhostTail.Object;
	}
}


void AGhostTrail::BeginPlay()
{
	Super::BeginPlay();
}

void AGhostTrail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsSpawned) return;

	if (!IsEnabled)
	{
		EnableCountDown -= DeltaTime;
		if (EnableCountDown <= 0.0f)
		{
			IsEnabled = true;
			UpdateMaterialParameters();
		}
		return;
	}

	FadeCountDown -= DeltaTime;
	if (FadeCountDown < 0)
	{
		Reset();
		return;
	}

	float CurrentOpacity = FMath::Max(FadeCountDown / FadeOutTime * InitialOpacity, MinOpacity);
	for (auto* Material : DynamicMaterials)
	{
		if (Material)
		{
			Material->SetScalarParameterValue("Opacity", CurrentOpacity);
		}
	}
}

void AGhostTrail::Init(USkeletalMeshComponent* Pawn, float FadeOutDuration, float EnableDelay, bool IsDynamicMaterial)
{
	PoseableMesh->SetSkinnedAssetAndUpdate(Pawn->GetSkeletalMeshAsset());
	PoseableMesh->CopyPoseFromSkeletalComponent(Pawn);
	PoseableMesh->SetRelativeScale3D(Pawn->GetRelativeScale3D());

	int32 MatCount = Pawn->GetNumMaterials();
	if (IsDynamicMaterial)
	{
		for (int32 i = 0; i < MatCount; i++)
		{
			if (UMaterialInterface* OriginalMaterial = Pawn->GetMaterial(i))
			{
				UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
				if (DynMaterial)
				{
					DynMaterial->SetScalarParameterValue("Opacity", 0.0f); // 초기에는 투명하게 시작
					DynamicMaterials.Add(DynMaterial);
					PoseableMesh->SetMaterial(i, DynMaterial);
				}
			}
		}
	}
	else
	{
		DynamicMaterials.Empty();
		for (int32 i = 0; i < MatCount; i++)
		{
			PoseableMesh->SetMaterial(i, GhostMaterial);
		}
	}

	FadeOutTime = FadeOutDuration;
	FadeCountDown = FadeOutDuration;
	EnableCountDown = EnableDelay;
	IsSpawned = true;
	IsEnabled = false;
}

void AGhostTrail::Reset()
{
	IsSpawned = false;
	IsEnabled = false;
	SetActorTickEnabled(false); // 리셋 시 틱 비활성화
	ClearMaterials();
}

void AGhostTrail::SetGhostColor(const FLinearColor& Color)
{
	GhostColor = Color;
	UpdateMaterialParameters();
}

void AGhostTrail::SetCustomMaterial(UMaterialInterface* NewMaterial)
{
	if (NewMaterial)
	{
		GhostMaterial = NewMaterial;
	}
}

void AGhostTrail::UpdateMaterialParameters()
{
	for (auto* Material : DynamicMaterials)
	{
		if (Material)
		{
			Material->SetVectorParameterValue("Color", GhostColor);
			Material->SetScalarParameterValue("Opacity", IsEnabled ? InitialOpacity : 0.0f);
		}
	}
}


void AGhostTrail::ClearMaterials()
{
	DynamicMaterials.Empty();
	for (int32 i = 0; i < PoseableMesh->GetNumMaterials(); i++)
	{
		PoseableMesh->SetMaterial(i, nullptr);
	}
}
