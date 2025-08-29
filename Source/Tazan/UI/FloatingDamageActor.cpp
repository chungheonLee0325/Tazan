#include "FloatingDamageActor.h"
#include "Components/WidgetComponent.h"
#include "Tazan/UI/Widget/FloatingDamageWidget.h"

AFloatingDamageActor::AFloatingDamageActor()
{
    PrimaryActorTick.bCanEverTick = true;

    DamageWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidget"));
    RootComponent = DamageWidget;

    DamageWidget->SetWidgetSpace(EWidgetSpace::Screen);
    DamageWidget->SetDrawAtDesiredSize(true);

    // UI 클래스 설정
    static ConstructorHelpers::FClassFinder<UFloatingDamageWidget> WidgetClassFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/_BluePrints/Widget/WB_FloatingDamageWidget.WB_FloatingDamageWidget_C'"));
    if (WidgetClassFinder.Succeeded())
    {
        DamageWidget->SetWidgetClass(WidgetClassFinder.Class);
    }
}

void AFloatingDamageActor::Initialize(float Damage, EFloatingDamageType DamageType, float Duration, float RiseSpeed)
{
    LifeTime = Duration;

    // 랜덤 오프셋 계산
    float RandomX = FMath::RandRange(-RandomOffsetRange, RandomOffsetRange);
    float RandomY = FMath::RandRange(-RandomOffsetRange, RandomOffsetRange);

    AddActorLocalOffset(FVector(RandomX, RandomY, 0.0f));

    if (UFloatingDamageWidget* Widget = Cast<UFloatingDamageWidget>(DamageWidget->GetUserWidgetObject()))
    {
        Widget->SetDamageInfo(Damage, DamageType);
        Widget->PlayFadeAnimation();
    }
}

void AFloatingDamageActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 설정된 방향으로 이동
    AddActorWorldOffset(FVector::UpVector * MovementSpeed * DeltaTime);

    CurrentLifeTime += DeltaTime;
    if (CurrentLifeTime >= LifeTime)
    {
        Destroy();
    }
} 