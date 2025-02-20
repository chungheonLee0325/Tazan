#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingDamageWidget.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Normal,
    PlayerDamaged,
    WeakPointDamage
};

UCLASS()
class TAZAN_API UFloatingDamageWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetDamageInfo(float Damage, EDamageType Type);
    void PlayFadeAnimation();

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* DamageText;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FadeOutAnimation;

    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    TMap<EDamageType, FLinearColor> DamageColors;
}; 