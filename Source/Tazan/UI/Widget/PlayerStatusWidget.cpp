#include "PlayerStatusWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UPlayerStatusWidget::UpdateHealth(float CurrentHP, float Delta, float MaxHP)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(CurrentHP / MaxHP);
    }
    
    if (HealthText)
    {
        HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentHP, MaxHP)));
    }
}

void UPlayerStatusWidget::UpdateStamina(float CurrentStamina, float Delta, float MaxStamina)
{
    if (StaminaBar)
    {
        StaminaBar->SetPercent(CurrentStamina / MaxStamina);
    }
    
    if (StaminaText)
    {
        StaminaText->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentStamina, MaxStamina)));
    }
} 