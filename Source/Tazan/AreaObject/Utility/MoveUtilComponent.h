#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Curves/CurveFloat.h"
#include "TimerManager.h"
#include "Tazan/ResourceManager/KazanGameType.h" // EMovementInterpolationType
#include "MoveUtilComponent.generated.h"

class AAreaObject;
class APlayer_Kazan;
class ABaseMonster;
class ULockOnComponent;

// 종료 사유
UENUM(BlueprintType)
enum class EMoveFinishReason : uint8
{
	Reached, Blocked, Canceled, Replaced, Timeout
};

// 이동 의도
UENUM(BlueprintType)
enum class EMoveIntent : uint8
{
	ToLocation,
	InFacing,
	TowardActor
};

// 속도학 표현
UENUM(BlueprintType)
enum class EKinematics : uint8
{
	// 거리/속도 → 내부 Duration 환산
	BySpeed,
	// 고정 시간
	ByDuration
};

// InFacing 방향
UENUM(BlueprintType)
enum class ERelMoveDir : uint8
{
	Forward, Backward, Left, Right,
	// 타깃 방향(Owner→TargetActor)
	Target,
	CustomYaw
};

// Toward 정책: 정지거리까지 / 고정 이동량
UENUM(BlueprintType)
enum class ETowardPolicy : uint8
{
	// StopDistance 지점까지 (시작 거리 의존)
	ReachStopDistance,
	// 타깃 방향으로 항상 일정 거리
	FixedTravel
};

// 시작 스펙
USTRUCT(BlueprintType)
struct FAreaMoveSpec
{
	GENERATED_BODY()
	;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMoveIntent Intent = EMoveIntent::InFacing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKinematics Kinematics = EKinematics::BySpeed;

	// 공통
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementInterpolationType Interp = EMovementInterpolationType::EaseOut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStickToGround = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSlideOnBlock = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SourceId = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> Curve = nullptr;

	// 속도/시간
	// BySpeed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	float Speed = 0.f;
	// ByDuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	float Duration = 0.f;
	// >0 → Timeout
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	float MaxDuration = 0.f;

	// ToLocation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation = FVector::ZeroVector;

	// InFacing
	// 주로 BySpeed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	float Distance = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERelMoveDir FacingDir = ERelMoveDir::Forward;
	// CustomYaw용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YawOffsetDeg = 0.f;
	// InFacing-Target일 때 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> TargetActor;

	// Toward
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> TowardActor;
	// ReachStopDistance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	float StopDistance = 0.f;
	// 정책/고정 이동량
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETowardPolicy TowardPolicy = ETowardPolicy::ReachStopDistance;
	// FixedTravel(+BySpeed)일 때
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	float TravelDistance = 0.f;
};

// 진행 중 업데이트
USTRUCT(BlueprintType)
struct FAreaMoveUpdate
{
	GENERATED_BODY()
	;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SourceId = 0;

	// 목표 변경
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRetarget = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector NewTargetLocation = FVector::ZeroVector;

	// 속도/시간 변경
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChangeKinetics = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EKinematics NewKinematics = EKinematics::BySpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	float NewSpeedOrDuration = 0.f;
};

// 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoveFinished, EMoveFinishReason, Reason, int32, SourceId);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TAZAN_API UMoveUtilComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMoveUtilComponent();

	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void StartMove(const FAreaMoveSpec& Spec);
	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void UpdateMove(const FAreaMoveUpdate& Update);
	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void StopMoveBySourceId(int32 SourceId, EMoveFinishReason Reason = EMoveFinishReason::Canceled);
	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void StopAllMoves(EMoveFinishReason Reason = EMoveFinishReason::Canceled);

	UFUNCTION(BlueprintPure, Category="MoveUtil")
	bool IsMovementActive() const { return MovementState.bIsActive; }

	UFUNCTION(BlueprintPure, Category="MoveUtil")
	int32 GetActiveSourceId() const { return MovementState.SourceId; }

	// 디버그/튜닝
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveUtil|Debug")
	bool bShowDebugPath = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveUtil|Tuning", meta=(ClampMin="0"))
	float MinUpdateInterval = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveUtil|Ground", meta=(ClampMin="0"))
	float GroundTraceUp = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveUtil|Ground", meta=(ClampMin="0"))
	float GroundTraceDown = 100.f;

	UPROPERTY(BlueprintAssignable, Category="MoveUtil")
	FOnMoveFinished OnMoveFinished;

	// ---- Helpers (static) ----
	/** 플레이어=LockOn, 몬스터=Aggro 우선으로 타깃 해석 */
	UFUNCTION(BlueprintPure, Category="MoveUtil|Helpers")
	static AActor* ResolveAreaTarget(AAreaObject* Area, bool bUseLockOn);

	/** Spec에서 비어있는 타깃 필드를 자동 보완 (Toward / InFacing(Target)) */
	UFUNCTION(BlueprintCallable, Category="MoveUtil|Helpers")
	static void AutoResolveTargetsIfNeeded(FAreaMoveSpec& Spec, AAreaObject* AreaObject,
	                                       bool bUseLockOnToward = true, bool bUseLockOnFacing = true);

	/** Toward용: 현재 프레임 기준 목표 위치(정지거리+캡슐합 보정) 계산 */
	UFUNCTION(BlueprintPure, Category="MoveUtil|Helpers")
	static FVector ComputeTowardTargetPos(AActor* Owner, AActor* Target, float StopDistance);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	struct FMovementState
	{
		bool bIsActive = false;

		// 보관
		FAreaMoveSpec Spec;

		// 시간/경로
		float CurrentTime = 0.f;
		float Duration = 0.f;
		float AccumulatedDelta = 0.f;

		FVector StartLoc = FVector::ZeroVector;
		FVector TargetLoc = FVector::ZeroVector;
		float TotalDistance = 0.f;

		// 편의
		int32 SourceId = 0;
		int32 Priority = 0;
		bool bSlideOnBlock = true;

		FTimerHandle TimeoutHandle;
		void Reset() { *this = FMovementState(); }
	} MovementState;

	// 내부 구현
	void BeginNewMovement(const FAreaMoveSpec& Spec);
	void StopMovement(EMoveFinishReason Reason);
	void StartTimeoutTimerIfNeeded(const FAreaMoveSpec& Spec);
	void ClearTimeoutTimer();

	// 타깃 계산
	void ComputeFacingTarget(const FAreaMoveSpec& Spec, FVector& OutTarget) const;
	void ComputeTowardTarget(const FAreaMoveSpec& Spec, FVector& OutTarget) const;
	float GetCombinedCapsuleRadius2D(const AActor* A, const AActor* B) const;

	// 보간/이동
	float CalcInterpAlpha(float RawAlpha, EMovementInterpolationType InterpType, const UCurveFloat* Curve) const;
	void ApplyGroundStick(FVector& InOutPosition) const;
	bool TryMoveOwner(const FVector& Delta, FHitResult& OutHit) const;

	// 디버그
	void DrawDebug() const;
};
