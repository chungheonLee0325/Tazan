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
#include "Components/WidgetComponent.h"
#include "Tazan/AreaObject/Base/AreaObject.h"


// Sets default values for this component's properties
ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	LockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	LockOnWidget->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidget->SetDrawAtDesiredSize(true);

	// 위젯 클래스 로드 및 생성
	static ConstructorHelpers::FClassFinder<ULockOnWidget> WidgetClassFinder(
		TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/_BluePrints/Widget/WB_LockOnWidget.WB_LockOnWidget_C'"));
	if (WidgetClassFinder.Succeeded())
	{
		LockOnWidget->SetWidgetClass(WidgetClassFinder.Class);
		//WidgetClass = WidgetClassFinder.Class;
	}
}


// Called when the game starts
void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	m_Owner = Cast<AAreaObject>(GetOwner());
	m_PlayerController = Cast<APlayerController>(m_Owner->GetController());
	InitializeLockOnWidget();
}

void ULockOnComponent::InitializeLockOnWidget()
{
	if (!m_Owner) return;
	if (!m_PlayerController) return;
	//LockOnWidget = CreateWidget<ULockOnWidget>(m_PlayerController, WidgetClass);
	if (LockOnWidget)
	{
		//LockOnWidget->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		LockOnWidget->SetWidgetSpace(EWidgetSpace::Screen);
		LockOnWidget->SetDrawSize(FVector2D(100.f, 100.f));
		LockOnWidget->AttachToComponent(m_Owner->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		LockOnWidget->RegisterComponent();
		LockOnWidget->SetVisibility(true);
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
	if (CurrentTarget != nullptr)
	{
		// ToDo : UI 완성시 삭제
		DrawDebugSphere(GetWorld(), CurrentTarget->GetActorLocation(), 10, 40, FColor::Red, false, 0.01f);
	}
}

bool ULockOnComponent::ToggleLockOn()
{
	if (!bIsLockOnMode)
	{
		// 락온 시작
		AAreaObject* NewTarget = FindBestTarget();
		if (NewTarget)
		{
			CurrentTarget = NewTarget;
			bIsLockOnMode = true;
			//OnLockOnStateChanged.Broadcast(true, CurrentTarget);

			// UI 표시
			if (LockOnWidget)
			{
				//LockOnWidget->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
				//LockOnWidget->AttachToComponent(NewTarget, FAttachmentTransformRules::KeepRelativeTransform);
				LockOnWidget->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				LockOnWidget->AttachToComponent(NewTarget->GetMesh(),
				                                FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				LockOnWidget->SetVisibility(true);
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
		bIsLockOnMode = false;
		CurrentTarget = nullptr;
		//OnLockOnStateChanged.Broadcast(false, nullptr);

		// UI 숨기기
		if (LockOnWidget)
		{
			LockOnWidget->K2_DetachFromComponent();
			LockOnWidget->SetVisibility(false);
			//LockOnWidget->ClearTargets();
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
	if (!CurrentTarget || !IsTargetIsDead(CurrentTarget))
	{
		// 현재 타겟이 죽으면 다른 대상 락온 찾기 
		ToggleLockOn();
		ToggleLockOn();
	}
	if (!CurrentTarget || !IsTargetDistanceValid(CurrentTarget))
	{
		// 현재 타겟이 유효하지 않으면 락온 해제
		ToggleLockOn();
	}

	// 필요한 경우 여기에 추가 업데이트 로직
}

bool ULockOnComponent::IsTargetValid(AAreaObject* Target) const
{
	if (!IsTargetIsDead(Target))
		return false;
	if (!IsTargetDistanceValid(Target))
		return false;
	// 시야각 체크
	// Get camera location and direction
	FVector CameraLocation;
	FRotator CameraRotation;
	m_PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector CameraForward = CameraRotation.Vector();

	// Calculate direction to target from camera's perspective
	FVector DirectionToTarget = (Target->GetActorLocation() - CameraLocation).GetSafeNormal();

	// Angle check using camera's forward vector
	float DotResult = FVector::DotProduct(CameraForward, DirectionToTarget);

	// You can define a minimum dot product threshold corresponding to your desired angle
	// For example, 0.5 corresponds to approximately 60 degrees field of view (30 degrees on each side)
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
		CurrentTarget = BestNewTarget;
		//OnTargetSwitched.Broadcast(CurrentTarget);
		UpdateLockOnWidget();
	}
}

void ULockOnComponent::UpdateLockOnWidget()
{
	if (!LockOnWidget) return;

	// 메인 타겟 마커 업데이트
	//LockOnWidget->UpdateTargetMarker(CurrentTarget);

	// 잠재적 타겟 마커 업데이트
	//UpdatePotentialTargets();
}
