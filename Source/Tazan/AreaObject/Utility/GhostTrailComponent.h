#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GhostTrailComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TAZAN_API UGhostTrailComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGhostTrailComponent();

    UFUNCTION(BlueprintCallable, Category = "Ghost Trail")
    void StartGhostTrail(float Duration = 0.5f, float SpawnInterval = 0.1f, float EnableDelay = 0.0f);
    
    UFUNCTION(BlueprintCallable, Category = "Ghost Trail")
    void StopGhostTrail();

    UFUNCTION(BlueprintCallable, Category = "Ghost Trail")
    void CreateSingleGhostTrail(float FadeOutDuration = 0.5f, float EnableDelay = 0.0f);

    UFUNCTION(BlueprintCallable, Category = "Ghost Trail")
    void SetGhostColor(const FLinearColor& Color);

    UFUNCTION(BlueprintCallable, Category = "Ghost Trail")
    void SetCustomMaterial(UMaterialInterface* NewMaterial);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY()
    class USkeletalMeshComponent* OwnerMesh;
    
    UPROPERTY(EditDefaultsOnly, Category = "Ghost Trail")
    FLinearColor GhostColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    UPROPERTY(EditDefaultsOnly, Category = "Ghost Trail")
    UMaterialInterface* CustomMaterial;
    
    FTimerHandle SpawnTimerHandle;
    bool bIsActive = false;

    void SpawnGhostTrail(float FadeOutDuration, float EnableDelay);
};