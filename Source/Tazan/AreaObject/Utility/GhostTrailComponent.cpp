 #include "GhostTrailComponent.h"
#include "Tazan/Contents/Subsystems/GhostTrailSubsystem.h"
#include "Tazan/AreaObject/Utility/GhostTrail.h"

UGhostTrailComponent::UGhostTrailComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UGhostTrailComponent::BeginPlay()
{
    Super::BeginPlay();
    
    AActor* Owner = GetOwner();
    if (Owner)
    {
        OwnerMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
    }
}

void UGhostTrailComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    StopGhostTrail();
}

void UGhostTrailComponent::StartGhostTrail(float Duration, float SpawnInterval, float EnableDelay)
{
    if (!OwnerMesh || bIsActive) return;

    bIsActive = true;
    
    // 첫 번째 잔상 즉시 생성
    SpawnGhostTrail(Duration, EnableDelay);

    // 타이머로 주기적 생성
    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        [this, Duration, EnableDelay]()
        {
            SpawnGhostTrail(Duration, EnableDelay);
        },
        SpawnInterval,
        true
    );
}

void UGhostTrailComponent::StopGhostTrail()
{
    if (!bIsActive) return;

    bIsActive = false;
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
    }
}

void UGhostTrailComponent::CreateSingleGhostTrail(float FadeOutDuration, float EnableDelay)
{
    if (!OwnerMesh) return;
    SpawnGhostTrail(FadeOutDuration, EnableDelay);
}

void UGhostTrailComponent::SpawnGhostTrail(float FadeOutDuration, float EnableDelay)
{
    if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
    {
        if (UGhostTrailSubsystem* GhostSystem = GameInstance->GetSubsystem<UGhostTrailSubsystem>())
        {
            if (AGhostTrail* GhostTrail = GhostSystem->GetGhostTrail())
            {
                GhostTrail->SetActorTransform(OwnerMesh->GetComponentTransform());
                GhostTrail->Init(OwnerMesh, FadeOutDuration, EnableDelay);
                GhostTrail->SetActorHiddenInGame(false);
            }
        }
    }
}

 void UGhostTrailComponent::SetGhostColor(const FLinearColor& Color)
 {
    
 }

 void UGhostTrailComponent::SetCustomMaterial(UMaterialInterface* NewMaterial)
 {
 }
