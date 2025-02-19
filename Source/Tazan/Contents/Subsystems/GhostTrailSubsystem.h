 #pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tazan/AreaObject/Utility/GhostTrail.h"
#include "GhostTrailSubsystem.generated.h"

UCLASS()
class TAZAN_API UGhostTrailSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    AGhostTrail* GetGhostTrail();
    void ReturnGhostTrail(AGhostTrail* Trail);

    UFUNCTION(BlueprintCallable, Category = "Ghost Trail")
    void SetPoolSize(int32 NewSize);

private:
    void InitializePool();
    AGhostTrail* CreateGhostTrail();

    UPROPERTY()
    TArray<AGhostTrail*> GhostTrailPool;
    
    int32 PoolSize = 20;
    bool bIsInitialized = false;
};