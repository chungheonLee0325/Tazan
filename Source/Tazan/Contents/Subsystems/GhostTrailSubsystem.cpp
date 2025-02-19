#include "GhostTrailSubsystem.h"
#include "Tazan/Utilities/LogMacro.h"

void UGhostTrailSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitializePool();
}

void UGhostTrailSubsystem::Deinitialize()
{
    Super::Deinitialize();
    GhostTrailPool.Empty();
}

void UGhostTrailSubsystem::InitializePool()
{
    if (bIsInitialized) return;

    for (int32 i = 0; i < PoolSize; ++i)
    {
        AGhostTrail* Trail = CreateGhostTrail();
        if (Trail)
        {
            Trail->SetActorHiddenInGame(true);
            GhostTrailPool.Add(Trail);
        }
    }
    
    bIsInitialized = true;
}

AGhostTrail* UGhostTrailSubsystem::GetGhostTrail()
{
    for (AGhostTrail* Trail : GhostTrailPool)
    {
        if (Trail && !Trail->IsSpawned)
        {
            return Trail;
        }
    }

    // 풀이 부족한 경우 새로 생성
    AGhostTrail* NewTrail = CreateGhostTrail();
    if (NewTrail)
    {
        GhostTrailPool.Add(NewTrail);
        return NewTrail;
    }

    return nullptr;
}

void UGhostTrailSubsystem::ReturnGhostTrail(AGhostTrail* Trail)
{
    if (Trail)
    {
        Trail->SetActorHiddenInGame(true);
        Trail->IsSpawned = false;
    }
}

AGhostTrail* UGhostTrailSubsystem::CreateGhostTrail()
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    return World->SpawnActor<AGhostTrail>(AGhostTrail::StaticClass());
}

void UGhostTrailSubsystem::SetPoolSize(int32 NewSize)
{
    PoolSize = NewSize;
    if (bIsInitialized)
    {
        InitializePool();
    }
} 