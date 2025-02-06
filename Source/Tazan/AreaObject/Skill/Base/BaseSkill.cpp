// BaseSkill.cpp

#include "BaseSkill.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

UBaseSkill::UBaseSkill()
    : m_CurrentPhase(ESkillPhase::Ready)
    , m_CurrentPhaseTime(0.0f)
    , m_CurrentCooldown(0.0f)
    , m_Caster(nullptr)
    , m_Target(nullptr)
{
}

bool UBaseSkill::CanCast(ABaseMonster* Caster, const AActor* Target) const
{
    if (!Caster || !Target) return false;

    // 스킬 상태 체크
    if (m_CurrentPhase != ESkillPhase::Ready) return false;
    if (m_CurrentCooldown > 0.0f) return false;

    // 사거리 체크
    return IsInRange(Caster, Target);
}

void UBaseSkill::OnCastStart(ABaseMonster* Caster, const AActor* Target)
{
    if (!Caster || !Target) return;

    m_Caster = Caster;
    m_Target = Target;

    // Prepare 페이즈로 전환
    if (SkillData.PrepareTime > 0.0f)
    {
        UpdatePhase(ESkillPhase::Prepare);
        SpawnCastEffect();
    }
    else
    {
        UpdatePhase(ESkillPhase::Casting);
        SpawnCastEffect();
        SpawnProgressEffect();
    }
}

void UBaseSkill::UpdatePreparePhase(float DeltaTime)
{
}

void UBaseSkill::UpdateCastingPhase(float DeltaTime)
{
}

void UBaseSkill::UpdatePostCastPhase(float DeltaTime)
{
}

void UBaseSkill::OnCastTick(float DeltaTime)
{
    if (!m_Caster || !m_Target) return;

    m_CurrentPhaseTime -= DeltaTime;

    switch (m_CurrentPhase)
    {
    case ESkillPhase::Prepare:
        if (m_CurrentPhaseTime <= 0.0f)
        {
            UpdatePhase(ESkillPhase::Casting);
            SpawnProgressEffect();
        }
        else
        {
            UpdatePreparePhase(DeltaTime);
        }
        
        break;

    case ESkillPhase::Casting:
        if (m_CurrentPhaseTime <= 0.0f)
        {
            UpdatePhase(ESkillPhase::PostCast);
            SpawnEndEffect();
        }
        else
        {
            UpdateCastingPhase(DeltaTime);
        }
        break;

    case ESkillPhase::PostCast:
        if (m_CurrentPhaseTime <= 0.0f)
        {
            OnCastEnd();
        }
        else
        {
            UpdatePostCastPhase(DeltaTime);
        }
        break;

   // case ESkillPhase::Cooldown:
   //     if (m_CurrentPhaseTime <= 0.0f)
   //     {
   //         UpdatePhase(ESkillPhase::Ready);
   //     }
   //     break;
    }
}

void UBaseSkill::OnCastEnd()
{
    if (!m_Caster || !m_Target) return;

    m_Caster->ClearCurrentSkill();
    if (nullptr != m_NextSkill && m_Caster->CanCastSkill(m_NextSkill, m_Target))
    {
        m_NextSkill->OnSkillComplete = OnSkillComplete;
        m_Caster->CastSkill(m_NextSkill, m_Target);
    }
    else
    {
        if (OnSkillComplete.IsBound() == true)
        {
            OnSkillComplete.Execute();
            OnSkillComplete.Unbind();
        }
    }
    ClearEffects();
    UpdatePhase(ESkillPhase::Ready);
}

void UBaseSkill::CancelCast()
{
    if (!m_Caster) return;
    if (OnSkillComplete.IsBound() == true)
    {
        OnSkillComplete.Unbind();
    }
    if (m_CurrentPhase != ESkillPhase::Ready)
    {
        m_Caster->ClearCurrentSkill();
        ClearEffects();
        UpdatePhase(ESkillPhase::Ready);
    }
}

float UBaseSkill::GetPhaseProgress() const
{
    float totalTime = 0.0f;
    switch (m_CurrentPhase)
    {
    case ESkillPhase::Ready:
        return 1.0f;
    case ESkillPhase::Prepare:
        totalTime = SkillData.PrepareTime;
        break;
    case ESkillPhase::Casting:
        totalTime = SkillData.CastTime;
        break;
    case ESkillPhase::PostCast:
        totalTime = SkillData.PostCastTime;
        break;
    // case ESkillPhase::Cooldown:
    //     totalTime = SkillData.Cooldown;
    //     break;
    }

    if (totalTime <= 0.0f) return 1.0f;
    return 1.0f - (m_CurrentPhaseTime / totalTime);
}

float UBaseSkill::GetCooldownProgress() const
{
    if (SkillData.Cooldown <= 0.0f) return 1.0f;
    return 1.0f - (m_CurrentCooldown / SkillData.Cooldown);
}

bool UBaseSkill::IsInRange(const ABaseMonster* Caster, const AActor* Target) const
{
    if (!Caster || !Target) return false;
    
    float DistanceSquared = FVector::DistSquared(Caster->GetActorLocation(), Target->GetActorLocation());
    float RangeSquared = SkillData.CastRange * SkillData.CastRange;
    
    return DistanceSquared <= RangeSquared;
}

void UBaseSkill::Set_TargetPos(const FVector& M_LaunchPosition)
{
    m_TargetPos = M_LaunchPosition;
}

void UBaseSkill::UpdatePhase(ESkillPhase NewPhase)
{
    m_CurrentPhase = NewPhase;
    
    switch (NewPhase)
    {
    case ESkillPhase::Ready:
        m_CurrentPhaseTime = 0.0f;
        break;
    case ESkillPhase::Prepare:
        m_CurrentPhaseTime = SkillData.PrepareTime;
        break;
    case ESkillPhase::Casting:
        m_CurrentPhaseTime = SkillData.CastTime;
        break;
    case ESkillPhase::PostCast:
        m_CurrentPhaseTime = SkillData.PostCastTime;
        break;
    //case ESkillPhase::Cooldown:
    //    m_CurrentPhaseTime = SkillData.Cooldown;
    //    break;
    }

    OnPhaseChanged(NewPhase);
}

USceneComponent* UBaseSkill::GetAttachComponent() const
{
    if (!m_Caster) return nullptr;
    
    // 캐스터의 메시나 루트 컴포넌트를 반환
    USkeletalMeshComponent* MeshComponent = m_Caster->GetMesh();
    if (MeshComponent) return MeshComponent;
    
    return m_Caster->GetRootComponent();
}

void UBaseSkill::SpawnCastEffect()
{
    if (USceneComponent* AttachComponent = GetAttachComponent())
    {
        // Cascade Particle System
        if (SkillData.CastEffect)
        {
            UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(
                SkillData.CastEffect,
                AttachComponent,
                NAME_None,
                FVector::ZeroVector,
                FRotator::ZeroRotator,
                EAttachLocation::SnapToTarget
            );
            if (PSC)
            {
                ActiveParticleEffects.Add(PSC);
            }
        }

        // Niagara System
        if (SkillData.CastNiagaraEffect)
        {
            UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
                SkillData.CastNiagaraEffect,
                AttachComponent,
                NAME_None,
                FVector::ZeroVector,
                FRotator::ZeroRotator,
                EAttachLocation::SnapToTarget,
                true
            );
            if (NiagaraComp)
            {
                ActiveNiagaraEffects.Add(NiagaraComp);
            }
        }
    }
}

void UBaseSkill::SpawnProgressEffect()
{
    if (USceneComponent* AttachComponent = GetAttachComponent())
    {
        if (SkillData.ProgressEffect)
        {
            UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(
                SkillData.ProgressEffect,
                AttachComponent
            );
            if (PSC)
            {
                ActiveParticleEffects.Add(PSC);
            }
        }

        if (SkillData.ProgressNiagaraEffect)
        {
            UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
                SkillData.ProgressNiagaraEffect,
                AttachComponent,
                NAME_None,
                FVector::ZeroVector,
                FRotator::ZeroRotator,
                EAttachLocation::SnapToTarget,
                true
            );
            if (NiagaraComp)
            {
                ActiveNiagaraEffects.Add(NiagaraComp);
            }
        }
    }
}

void UBaseSkill::SpawnEndEffect()
{
    if (USceneComponent* AttachComponent = GetAttachComponent())
    {
        if (SkillData.EndEffect)
        {
            UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(
                SkillData.EndEffect,
                AttachComponent
            );
            if (PSC)
            {
                ActiveParticleEffects.Add(PSC);
            }
        }

        if (SkillData.EndNiagaraEffect)
        {
            UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
                SkillData.EndNiagaraEffect,
                AttachComponent,
                NAME_None,
                FVector::ZeroVector,
                FRotator::ZeroRotator,
                EAttachLocation::SnapToTarget,
                true
            );
            if (NiagaraComp)
            {
                ActiveNiagaraEffects.Add(NiagaraComp);
            }
        }
    }
}
void UBaseSkill::ClearEffects()
{
}

void UBaseSkill::OnPhaseChanged(ESkillPhase NewPhase)
{
}
