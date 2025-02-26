#include "LockOnWidget.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Base/AreaObject.h"

void ULockOnWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 메인 타겟 마커 초기 설정
    if (MainTargetMarker)
    {
        MainTargetMarker->SetVisibility(ESlateVisibility::Hidden);
        MainTargetMarker->SetBrushFromTexture(TargetMarkerTexture);
        MainTargetMarker->SetDesiredSizeOverride(MarkerSize);
    }
}

void ULockOnWidget::UpdateTargetMarker(AAreaObject* Target)
{
    if (!MainTargetMarker || !Target) 
    {
        if (MainTargetMarker)
        {
            MainTargetMarker->SetVisibility(ESlateVisibility::Hidden);
        }
        return;
    }

    MainTargetMarker->SetVisibility(ESlateVisibility::Visible);
}

void ULockOnWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
}

void ULockOnWidget::ClearTargets()
{
    if (MainTargetMarker)
    {
        MainTargetMarker->SetVisibility(ESlateVisibility::Hidden);
    }
} 