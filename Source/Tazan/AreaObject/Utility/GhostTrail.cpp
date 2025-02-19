// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostTrail.h"
#include "Components/PoseableMeshComponent.h"


AGhostTrail::AGhostTrail()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("POSEABLEMESH"));
	RootComponent = PoseableMesh;
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PoseMesh(
	//	TEXT("SkeletalMesh'/Game/PP801_P3/Meshes/Characters/Combines/SK_PP801P3_G.SK_PP801P3_G'"));
	//if (SK_PoseMesh.Succeeded())
	//{
	//	PoseableMesh->SetSkeletalMesh(SK_PoseMesh.Object);
	//}

	//ConstructorHelpers::FObjectFinder<UMaterialInstance> M_GhostTail(
	//	TEXT("MaterialInstanceConstant'/Game/Blueprints/Player/MI_GhostTail.MI_GhostTail'"));
	//if (M_GhostTail.Succeeded())
	//{
	//	GhostMaterial = M_GhostTail.Object;
	//}
}


void AGhostTrail::BeginPlay()
{
	Super::BeginPlay();
}

void AGhostTrail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsSpawned)
	{
		FadeCountDown -= DeltaTime;
		if (FadeCountDown < 0)
		{
			Destroy();
		}

		float CurrentOpacity = FMath::Max(FadeCountDown / FadeOutTime, 0.2f);

		// 모든 머티리얼의 투명도 업데이트
		for (auto* Material : DynamicMaterials)
		{
			if (Material)
			{
				Material->SetScalarParameterValue("Opacity", CurrentOpacity);
			}
		}
	}
}

void AGhostTrail::Init(USkeletalMeshComponent* Pawn)
{
	PoseableMesh->SetSkinnedAssetAndUpdate(Pawn->GetSkeletalMeshAsset());
	PoseableMesh->CopyPoseFromSkeletalComponent(Pawn);
	PoseableMesh->SetRelativeScale3D(Pawn->GetRelativeScale3D());

	int32 MatCount = Pawn->GetNumMaterials();
	for (int32 i = 0; i < MatCount; i++)
	{
		if (UMaterialInterface* OriginalMaterial = Pawn->GetMaterial(i))
		{
			// 원본 머티리얼의 동적 인스턴스 생성
			UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
			if (DynMaterial)
			{
				DynMaterial->SetScalarParameterValue("Opacity", 0.7f);
				DynamicMaterials.Add(DynMaterial);
				PoseableMesh->SetMaterial(i, DynMaterial);
			}
		}
	}

	FadeCountDown = FadeOutTime;
	IsSpawned = true;

	/*
PoseableMesh->CopyPoseFromSkeletalComponent(Pawn);
TArray<UMaterialInterface*> Mats = PoseableMesh->GetMaterials();

for (int i = 0; i < Mats.Num(); i++)
{
Materials.Add(UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), GhostMaterial));
PoseableMesh->SetMaterial(i, Materials[i]);
}
FadeCountDown = FadeOutTime;
IsSpawned = true;
 */
}

void AGhostTrail::InitByMaterials(USkeletalMeshComponent* Pawn, const TArray<UMaterialInterface*>& Materials)
{
	PoseableMesh->SetSkinnedAssetAndUpdate(Pawn->GetSkeletalMeshAsset());
	PoseableMesh->CopyPoseFromSkeletalComponent(Pawn);
	PoseableMesh->SetRelativeScale3D(Pawn->GetRelativeScale3D());

	// 기존 마테리얼 초기화
	int32 OriginMatCount = Pawn->GetNumMaterials();
	for (int32 i = 0; i < OriginMatCount; i++)
	{
		PoseableMesh->SetMaterial(i, nullptr);
	}

	int32 MatCount = Materials.Num();
	for (int32 i = 0; i < MatCount; i++)
	{
		if (UMaterialInterface* OriginalMaterial = Materials[i])
		{
			// 원본 머티리얼의 동적 인스턴스 생성
			UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
			if (DynMaterial)
			{
				DynMaterial->SetScalarParameterValue("Opacity", 0.7f);
				DynamicMaterials.Add(DynMaterial);
				PoseableMesh->SetMaterial(i, DynMaterial);
			}
		}
	}

	FadeCountDown = FadeOutTime;
	IsSpawned = true;
}
