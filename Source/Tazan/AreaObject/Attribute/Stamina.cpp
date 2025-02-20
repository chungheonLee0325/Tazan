#include "Stamina.h"

UStamina::UStamina()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UStamina::BeginPlay()
{
    Super::BeginPlay();
}

void UStamina::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 스태미나 자동 회복
    if (bCanRecover && m_Stamina < m_StaminaMax)
    {
        float recovery = m_RecoveryRate * DeltaTime;
        
        // 가드 중일 때 회복률 감소
        if (bIsGuarding)
        {
            recovery *= m_GuardRecoveryRateMultiplier;
        }
        
        IncreaseStamina(recovery);
    }
}

void UStamina::InitStamina(float StaminaMax, float RecoveryRate)
{
    m_StaminaMax = StaminaMax;
    m_Stamina = m_StaminaMax;
    m_RecoveryRate = RecoveryRate;
    OnStaminaChanged.Broadcast(m_Stamina, 0, m_StaminaMax);
}

float UStamina::DecreaseStamina(float Delta)
{
    if (Delta <= 0.0f) return m_Stamina;
    
    // 스태미나 감소
    float oldStamina = m_Stamina;
    m_Stamina = FMath::Clamp(m_Stamina - Delta, 0.0f, m_StaminaMax);
    
    if (!FMath::IsNearlyEqual(oldStamina, m_Stamina))
    {
        OnStaminaChanged.Broadcast(m_Stamina, -(oldStamina - m_Stamina), m_StaminaMax);
        
        // 회복 중지 및 딜레이 타이머 시작
        StopStaminaRecovery();
        GetWorld()->GetTimerManager().SetTimer(
            RecoveryDelayHandle,
            this,
            &UStamina::StartStaminaRecovery,
            m_RecoveryDelay,
            false
        );
    }
    
    return m_Stamina;
}

float UStamina::IncreaseStamina(float Delta)
{
    if (Delta <= 0.0f) return m_Stamina;
    
    float oldStamina = m_Stamina;
    m_Stamina = FMath::Clamp(m_Stamina + Delta, 0.0f, m_StaminaMax);
    
    if (!FMath::IsNearlyEqual(oldStamina, m_Stamina))
    {
        OnStaminaChanged.Broadcast(m_Stamina, m_Stamina - oldStamina, m_StaminaMax);
    }
    
    return m_Stamina;
}

void UStamina::StartStaminaRecovery()
{
    bCanRecover = true;
}

void UStamina::StopStaminaRecovery()
{
    bCanRecover = false;
}

void UStamina::SetGuardState(bool IsGuarding)
{
    this->bIsGuarding = IsGuarding;
} 