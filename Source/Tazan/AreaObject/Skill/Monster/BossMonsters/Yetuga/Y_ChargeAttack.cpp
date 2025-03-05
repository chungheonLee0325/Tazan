// Fill out your copyright notice in the Description page of Project Settings.


#include "Y_ChargeAttack.h"

#include "Tazan/AreaObject/Monster/Variants/BossMonsters/Yetuga/Yetuga.h"
#include "Tazan/Utilities/LogMacro.h"

UY_ChargeAttack::UY_ChargeAttack()
{
	MontageBlendTime = 0.3f;
}

void UY_ChargeAttack::OnCastStart(class AAreaObject* Caster, AAreaObject* Target)
{
	Super::OnCastStart(Caster, Target);
	this->OnSkillCancel.BindUObject(this,&UY_ChargeAttack::Stun);
}

void UY_ChargeAttack::CancelCast()
{
	Super::CancelCast();
}

void UY_ChargeAttack::OnCastTick(float DeltaTime)
{
	Super::OnCastTick(DeltaTime);
	
	FVector start = m_Caster->GetActorLocation();
	FVector forward = m_Caster->GetActorForwardVector();
	FVector end = start + forward * 300.f;
    
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(m_Caster);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, start, end, ECC_Visibility, QueryParams);
    
	// 디버그용 라인 그리기
	// FColor LineColor = bHit ? FColor::Red : FColor::Green;
	// DrawDebugLine(GetWorld(), start, bHit ? HitResult.Location : end, LineColor, false, 2.0f, 0, 2.0f);
    
	if (bHit)
	{
		float hitDist = FVector::Dist(start, HitResult.Location);

		if (hitDist <= 160.0f)
		{
			this->CancelCast();
			Stun();
		}
	}
}

void UY_ChargeAttack::Stun()
{
	AYetuga* yetuga = Cast<AYetuga>(m_Caster);
	if (yetuga)
	{
		yetuga->ChargeSkillStun();
	}
}
