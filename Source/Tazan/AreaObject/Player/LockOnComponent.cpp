// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "KismetTraceUtils.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Tazan/UI/Widget/LockOnWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/UI/Widget/PlayerStatusWidget.h"
#include "Tazan/Utilities/LogMacro.h"


// Sets default values for this component's properties
ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
    
	m_Owner = Cast<AAreaObject>(GetOwner());
	m_PlayerController = Cast<APlayerController>(m_Owner->GetController());
	
	if (m_PlayerController && LockOnWidgetClass)
	{
		LockOnWidgetInstance = CreateWidget<ULockOnWidget>(m_PlayerController, LockOnWidgetClass);
		if (LockOnWidgetInstance)
		{
			// 높은 Z-Order로 항상 위에 표시
			LockOnWidgetInstance->AddToViewport(100);
			// ViewPort에 추가하되 초기에는 숨김
			LockOnWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

// Called every frame
void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (bIsLockOnMode)
	{
		UpdateCurrentTarget();
		UpdateLockOnWidget();
	}
}

bool ULockOnComponent::ToggleLockOn()
{
	if (!bIsLockOnMode)
	{
		AAreaObject* NewTarget = FindBestTarget();
		if (NewTarget)
		{
			// 몬스터 HP 위젯 표시
			if (ABaseMonster* monster = Cast<ABaseMonster>(NewTarget))
			{
				monster->SetHPWidgetVisibility(true);
			}

			CurrentTarget = NewTarget;
			bIsLockOnMode = true;

			// UI 표시
			if (LockOnWidgetInstance)
			{
				LockOnWidgetInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
				UpdateLockOnWidget();
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		// 락온 해제
		if (ABaseMonster* monster = Cast<ABaseMonster>(CurrentTarget))
		{
			monster->SetHPWidgetVisibility(false);
		}

		bIsLockOnMode = false;
		CurrentTarget = nullptr;

		// UI 숨기기
		if (LockOnWidgetInstance)
		{
			LockOnWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	return true;
}

AAreaObject* ULockOnComponent::FindBestTarget()
{
	if (!m_Owner) return nullptr;

	UpdatePotentialTargets();

	if (PotentialTargets.Num() == 0) return nullptr;

	// 화면 중앙에 가장 가까운 타겟 찾기
	AAreaObject* BestTarget = nullptr;
	float BestScore = MAX_FLT;

	FVector CameraLocation;
	FRotator CameraRotation;
	m_Owner->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	for (AAreaObject* Target : PotentialTargets)
	{
		if (!IsTargetValid(Target)) continue;

		// 타겟의 스크린 위치 계산
		FVector TargetLocation = Target->GetActorLocation();
		FVector2D ScreenPosition;
		if (UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(),
		                                           TargetLocation,
		                                           ScreenPosition))
		{
			// 화면 중앙으로부터의 거리 계산
			int32 ViewportSizeX, ViewportSizeY;
			GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewportSizeX, ViewportSizeY);
			FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);
			float DistanceToCenter = FVector2D::Distance(ScreenPosition, ScreenCenter);

			// 실제 거리도 고려
			float ActualDistance = FVector::Distance(m_Owner->GetActorLocation(), TargetLocation);

			// 종합 점수 계산 (낮을수록 좋음)
			float Score = DistanceToCenter * 0.7f + ActualDistance * 0.3f;

			if (Score < BestScore)
			{
				BestScore = Score;
				BestTarget = Target;
			}
		}
	}

	return BestTarget;
}

void ULockOnComponent::UpdatePotentialTargets()
{
	if (!m_Owner)
		return;

	// 이전 타겟 목록 초기화
	PotentialTargets.Empty();

	// 플레이어 컨트롤러와 카메라 정보 획득
	if (m_PlayerController == nullptr)
	{
		m_PlayerController = Cast<APlayerController>(m_Owner->GetController());
	}
	if (!m_PlayerController)
		return;

	// 카메라 위치와 방향 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;
	m_PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector CameraForward = CameraRotation.Vector();

	// 방법 1: 구체 오버랩 테스트 후 시야각 필터링
	TArray<AActor*> OverlappedActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(m_Owner);

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		m_Owner->GetActorLocation(),
		LockOnLostRange, // 구체 반경
		TArray<TEnumAsByte<EObjectTypeQuery>>(), // 모든 객체 타입
		AAreaObject::StaticClass(), // 타겟 클래스
		IgnoreActors,
		OverlappedActors
	);

	// 오버랩된 모든 액터를 검사하여 카메라 시야각 내에 있는지 확인
	for (AActor* Actor : OverlappedActors)
	{
		AAreaObject* AreaObject = Cast<AAreaObject>(Actor);
		if (AreaObject)
		{
			// 카메라에서 타겟으로의 방향
			FVector DirectionToTarget = (AreaObject->GetActorLocation() - CameraLocation).GetSafeNormal();

			// 시야각 체크 (도트 프로덕트 사용)
			float DotResult = FVector::DotProduct(CameraForward, DirectionToTarget);

			// 시야각 임계값 (조정 가능)
			const float ViewAngleThreshold = 0.5f; // 약 60도 시야각

			if (DotResult >= ViewAngleThreshold)
			{
				// 시야 방해물 체크
				if (IsTargetVisible(AreaObject))
				{
					PotentialTargets.Add(AreaObject);
				}
			}
		}
	}

	// 타겟을 카메라 중앙에 가까운 순서로 정렬
	if (PotentialTargets.Num() > 0)
	{
		PotentialTargets.Sort([this, CameraLocation, CameraForward](const AActor& A, const AActor& B)
		{
			FVector DirToA = (A.GetActorLocation() - CameraLocation).GetSafeNormal();
			FVector DirToB = (B.GetActorLocation() - CameraLocation).GetSafeNormal();

			float DotA = FVector::DotProduct(CameraForward, DirToA);
			float DotB = FVector::DotProduct(CameraForward, DirToB);

			return DotA > DotB;
		});
	}
}

void ULockOnComponent::UpdateCurrentTarget()
{
    if (!CurrentTarget || !m_Owner) return;
    
    bool ShouldDisengage = false;
    FString DisengageReason;
    
    // 1. 타겟 사망
    if (CurrentTarget->IsDie())
    {
        ShouldDisengage = true;
        DisengageReason = "Target Died";
        
        // 죽은 타겟 근처의 다른 타겟으로 자동 전환 시도
        ToggleLockOn();
        ToggleLockOn();
    }
    
    // 2. 거리 체크 
    float Distance = FVector::Distance(m_Owner->GetActorLocation(), CurrentTarget->GetActorLocation());
    if (Distance > LockOnLostRange)
    {
        LostRangeTimer += GetWorld()->GetDeltaSeconds();
        if (LostRangeTimer > LostRangeTime)
        {
            ShouldDisengage = true;
            DisengageReason = "Out of Range";
        }
    }
    else
    {
        LostRangeTimer = 0.0f;
    }
    
    // 3. 시야 차단 체크
    if (!IsTargetVisible(CurrentTarget))
    {
        LostVisibilityTimer += GetWorld()->GetDeltaSeconds();
        if (LostVisibilityTimer > LostVisibilityTime)
        {
            ShouldDisengage = true;
            DisengageReason = "Lost Visibility";
        }
    }
    else
    {
        LostVisibilityTimer = 0.0f;
    }
    
    // 4. 각도 체크 (플레이어가 반대 방향을 보고 있을 때)
	FVector CameraLocation;
	FRotator CameraRotation;
	m_PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector CameraForward = CameraRotation.Vector();
	
    FVector ToTarget = (CurrentTarget->GetActorLocation() - CameraLocation).GetSafeNormal();
    float DotProduct = FVector::DotProduct(CameraForward, ToTarget);
    if (DotProduct < -0.5f) // 120도 이상 벗어남
    {
        BackTurnedTimer += GetWorld()->GetDeltaSeconds();
        if (BackTurnedTimer > BackTurnedTime)
        {
            ShouldDisengage = true;
            DisengageReason = "Player Turned Away";
        }
    }
    else
    {
        BackTurnedTimer = 0.0f;
    }
    
    if (ShouldDisengage)
    {
        UE_LOG(LogTemp, Warning, TEXT("Lock-on disengaged: %s"), *DisengageReason);
        ToggleLockOn(); // 록온 해제
    }
}

bool ULockOnComponent::IsTargetValid(AAreaObject* Target) const
{
	if (!IsTargetIsDead(Target))
		return false;
	if (!IsTargetDistanceValid(Target))
		return false;
	// 시야각 체크
	FVector CameraLocation;
	FRotator CameraRotation;
	m_PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector CameraForward = CameraRotation.Vector();

	FVector DirectionToTarget = (Target->GetActorLocation() - CameraLocation).GetSafeNormal();
	
	float DotResult = FVector::DotProduct(CameraForward, DirectionToTarget);
	
	const float MinDotProductThreshold = 0.5f; // Adjust this value as needed

	if (DotResult < MinDotProductThreshold)
		return false;


	// 시야 방해물 체크
	return IsTargetVisible(Target);
}

bool ULockOnComponent::IsTargetIsDead(AAreaObject* Target) const
{
	if (!Target || !m_Owner) return false;
	if (Target->IsDie() || m_Owner->IsDie()) return false;
	return true;
}

bool ULockOnComponent::IsTargetDistanceValid(AAreaObject* Target) const
{
	if (!Target || !m_Owner) return false;


	// 거리 체크
	float Distance = FVector::Distance(m_Owner->GetActorLocation(), Target->GetActorLocation());
	if (Distance > LockOnLostRange) return false;

	return true;
}

bool ULockOnComponent::IsTargetVisible(AAreaObject* Target) const
{
	if (!Target || !m_Owner) return false;

	//return true;
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(m_Owner);

	FVector StartTrace = m_Owner->GetActorLocation();
	FVector EndTrace = Target->GetActorLocation();

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace,
	                                                 ECC_Visibility, QueryParams);

	// 방해물이 없거나, 방해물이 타겟인 경우 true 반환
	return !bHit || HitResult.GetActor() == Target;
}

void ULockOnComponent::SwitchTarget(const FVector2D& Direction)
{
	if (!bIsLockOnMode || !CurrentTarget || Direction.IsNearlyZero()) return;

	UpdatePotentialTargets();

	AAreaObject* BestNewTarget = nullptr;
	float BestScore = MAX_FLT;

	FVector CurrentTargetLocation = CurrentTarget->GetActorLocation();

	for (AAreaObject* PotentialTarget : PotentialTargets)
	{
		if (PotentialTarget == CurrentTarget) continue;
		if (!IsTargetValid(PotentialTarget)) continue;

		FVector DirectionToTarget = PotentialTarget->GetActorLocation() - CurrentTargetLocation;
		DirectionToTarget.Z = 0; // 수직 거리는 무시
		DirectionToTarget.Normalize();

		FVector2D DirectionToTarget2D(DirectionToTarget.X, DirectionToTarget.Y);
		float DotProduct = FVector2D::DotProduct(Direction.GetSafeNormal(), DirectionToTarget2D);

		// 방향이 일치하고 데드존보다 큰 경우에만 고려
		if (DotProduct > SwitchTargetDeadzone)
		{
			float Distance = FVector::Distance(CurrentTargetLocation, PotentialTarget->GetActorLocation());
			float Score = Distance * (2.0f - DotProduct); // 거리와 방향 모두 고려

			if (Score < BestScore)
			{
				BestScore = Score;
				BestNewTarget = PotentialTarget;
			}
		}
	}

	if (BestNewTarget)
	{
		ABaseMonster* prevTarget = Cast<ABaseMonster>(CurrentTarget);
		if (prevTarget != nullptr)
		{
			prevTarget->SetHPWidgetVisibility(false);
		}
		ABaseMonster* newTarget = Cast<ABaseMonster>(BestNewTarget);
		if (newTarget != nullptr)
		{
			newTarget->SetHPWidgetVisibility(true);
		}

		CurrentTarget = BestNewTarget;
		//OnTargetSwitched.Broadcast(CurrentTarget);
		UpdateLockOnWidget();
	}
}

void ULockOnComponent::UpdateLockOnWidget()
{
    if (!LockOnWidgetInstance || !CurrentTarget || !m_PlayerController) return;
    
    // 타겟 위치 계산
    FVector TargetWorldLocation = CurrentTarget->GetActorLocation();
    
    // 스크린 좌표로 변환
    FVector2D ScreenPosition;
    bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(m_PlayerController, TargetWorldLocation, ScreenPosition);
    
    if (bIsOnScreen)
    {
        // 화면 내에 있을 때
        LockOnWidgetInstance->SetPositionInViewport(ScreenPosition);
        LockOnWidgetInstance->UpdateTargetMarker(CurrentTarget);
        LockOnWidgetInstance->SetRenderOpacity(1.0f);
    }
    else
    {
        // 화면 밖에 있을 때 - 가장자리에 인디케이터 표시
        int32 ViewportSizeX, ViewportSizeY;
        m_PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
        FVector2D ViewportCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);
        
        // 화면 중앙에서 타겟 방향으로의 벡터
        FVector2D DirectionToTarget = ScreenPosition - ViewportCenter;
        DirectionToTarget.Normalize();
        
        // 화면 가장자리 위치 계산
        const float EdgeMargin = 80.0f;
        float MaxX = (ViewportSizeX * 0.5f) - EdgeMargin;
        float MaxY = (ViewportSizeY * 0.5f) - EdgeMargin;
        
        // 가장자리와의 교점 계산
        float ScaleX = FMath::Abs(DirectionToTarget.X) > 0.001f ? MaxX / FMath::Abs(DirectionToTarget.X) : MAX_FLT;
        float ScaleY = FMath::Abs(DirectionToTarget.Y) > 0.001f ? MaxY / FMath::Abs(DirectionToTarget.Y) : MAX_FLT;
        float Scale = FMath::Min(ScaleX, ScaleY);
        
        FVector2D EdgePosition = ViewportCenter + DirectionToTarget * Scale;
        
        LockOnWidgetInstance->SetPositionInViewport(EdgePosition);
        LockOnWidgetInstance->UpdateTargetMarker(CurrentTarget);
        LockOnWidgetInstance->SetRenderOpacity(0.6f); // 화면 밖 타겟은 반투명하게
    }
}