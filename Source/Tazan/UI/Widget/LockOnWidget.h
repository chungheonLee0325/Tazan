#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LockOnWidget.generated.h"

class AAreaObject;

UCLASS()
class TAZAN_API ULockOnWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    
    // 타겟 마커 업데이트
    void UpdateTargetMarker(AAreaObject* Target);
    void ClearTargets();

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    UPROPERTY(meta = (BindWidget))
    class UImage* MainTargetMarker;
    
    UPROPERTY(EditAnywhere, Category = "UI")
    UTexture2D* TargetMarkerTexture;
    
    UPROPERTY(EditAnywhere, Category = "UI")
    FVector2D MarkerSize = FVector2D(32.0f, 32.0f);
}; 