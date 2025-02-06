// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaObject.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

#include "Tazan/AreaObject/Attribute/Condition.h"
#include "Tazan/AreaObject/Attribute/Health.h"
#include "Tazan/Contents/TazanGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAreaObject::AAreaObject()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Health Component 생성
	m_Health = CreateDefaultSubobject<UHealth>(TEXT("Health"));

	// Condition Component 생성
	m_Condition = CreateDefaultSubobject<UCondition>(TEXT("Condition"));
}


// Called when the game starts or when spawned
void AAreaObject::BeginPlay()
{
	Super::BeginPlay();

	// 데이터 초기화
	dt_AreaObject = Cast<UTazanGameInstance>(GetGameInstance())->GetDataAreaObject(m_AreaObjectID);

	// Health 초기화 By Data
	float hpMax = 1.0f;
	if (dt_AreaObject != nullptr)
	{
		hpMax = dt_AreaObject->HPMax;
	}
	
	m_Health->InitHealth(hpMax);
}

void AAreaObject::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called every frame
void AAreaObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAreaObject::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAreaObject::CalcDamage(float Damage, AActor* Caster, AActor* Target, bool IsPointDamage)
{
	
	if (false == IsPointDamage)
	{
		UGameplayStatics::ApplyDamage(Target,
			Damage,
			GetController(),
			this,
			UDamageType::StaticClass());
	}
	else
	{
		//ToDo : 추가 구현
		//UGameplayStatics::ApplyPointDamage();
	}
}

float AAreaObject::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator,
                              AActor* DamageCauser)
{
	if (IsDie() || HasCondition(EConditionType::Invincible))
		return 0.0f;
		
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if ( FMath::IsNearlyZero(IncreaseHP(-ActualDamage)))
	{
		if(true== ExchangeDead())
		{
			OnDie();
		}
	}
	
	return ActualDamage;
}

void AAreaObject::OnDie()
{
}

void AAreaObject::OnKill()
{
}

void AAreaObject::OnRevival()
{
}

bool AAreaObject::AddCondition(EConditionType Condition) const
{
	return m_Condition->AddCondition(Condition);
}
bool AAreaObject::RemoveCondition(EConditionType Condition) const
{
	return m_Condition->RemoveCondition(Condition);
}
bool AAreaObject::HasCondition(EConditionType Condition) const
{
	return m_Condition->HasCondition(Condition);
}
bool AAreaObject::ExchangeDead() const
{
	return m_Condition->ExchangeDead();
}

float AAreaObject::IncreaseHP(float Delta) const
{
	return m_Health->IncreaseHP(Delta);
}

void AAreaObject::SetHPByRate(float Rate) const
{
	m_Health->SetHPByRate(Rate);
}

float AAreaObject::GetHP() const
{
	return m_Health->GetHP();
}




