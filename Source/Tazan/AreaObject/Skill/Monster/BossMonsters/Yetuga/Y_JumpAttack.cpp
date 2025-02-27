// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_JumpAttack.h"

#include "Kismet/GameplayStatics.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/Utilities/LogMacro.h"

UY_JumpAttack::UY_JumpAttack()
{
}

void UY_JumpAttack::OnCastStart(class AAreaObject* Caster, AAreaObject* Target)
{
	Super::OnCastStart(Caster, Target);
}

void UY_JumpAttack::OnCastTick(float DeltaTime)
{
	Super::OnCastTick(DeltaTime);
}

void UY_JumpAttack::OnCastFire()
{
	FVector targetLoc = m_Target->GetActorLocation();
	FVector startLoc = m_Caster->GetActorLocation();
	float arcValue = 2.5f;

	FVector outVelocity = FVector::ZeroVector;
	if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(m_Caster, outVelocity, startLoc, targetLoc, GetWorld()->GetGravityZ(), arcValue))
	{
	     FPredictProjectilePathParams predictParams(20.0f, startLoc, outVelocity, 15.0f);
		
	     // predictParams.DrawDebugTime = 15.0f;    
	     // predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;
		
	     predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
	     FPredictProjectilePathResult result;
	     UGameplayStatics::PredictProjectilePath(this, predictParams, result);

		m_Caster->LaunchCharacter(outVelocity,true,true);
	}
}
