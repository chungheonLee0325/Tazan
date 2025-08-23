#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Curves/CurveFloat.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "MoveUtilComponent.generated.h"

UENUM(BlueprintType)
enum class EMoveFinishReason : uint8
{
	Reached UMETA(DisplayName = "Reached"),
	Blocked UMETA(DisplayName = "Blocked"),
	Canceled UMETA(DisplayName = "Canceled"),
	Replaced UMETA(DisplayName = "Replaced"),
	Timeout UMETA(DisplayName = "Timeout"),
};

UENUM(BlueprintType)
enum class EPMMovementMode : uint8
{
	Duration UMETA(DisplayName = "Duration Based"),
	Speed UMETA(DisplayName = "Speed Based")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoveFinished, EMoveFinishReason, Reason, int32, SourceId);

USTRUCT(BlueprintType)
struct FMovementState
{
	GENERATED_BODY()

	UPROPERTY()
	bool bIsActive = false;

	UPROPERTY()
	float CurrentTime = 0.f;

	// seconds
	UPROPERTY()
	float Duration = 0.f;

	// for Speed mode
	UPROPERTY()
	float Speed = 0.f;

	UPROPERTY()
	FVector StartPos = FVector::ZeroVector;

	UPROPERTY()
	FVector TargetPos = FVector::ZeroVector;

	UPROPERTY()
	EPMMovementMode MovementMode = EPMMovementMode::Duration;

	UPROPERTY()
	EMovementInterpolationType InterpType = EMovementInterpolationType::Linear;

	// ---- Extended controls ----
	UPROPERTY()
	bool bStickToGround = false;

	UPROPERTY()
	float TraceUp = 60.f;

	UPROPERTY()
	float TraceDown = 120.f;

	UPROPERTY()
	bool bSlideOnBlock = true;

	UPROPERTY()
	int32 Priority = 100;

	UPROPERTY()
	int32 SourceId = 0;

	UPROPERTY()
	TObjectPtr<UCurveFloat> Curve = nullptr;

	bool IsActive() const { return bIsActive; }

	float GetProgress() const
	{
		return (Duration > KINDA_SMALL_NUMBER)
			       ? FMath::Clamp(CurrentTime / Duration, 0.f, 1.f)
			       : 1.f;
	}

	void Stop() { bIsActive = false; }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TAZAN_API UMoveUtilComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMoveUtilComponent();

	// --- High-level API (backward compatible) ---
	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void MoveActorTo(const FVector& Target, float Duration,
	                 EMovementInterpolationType Interp = EMovementInterpolationType::Linear,
	                 bool bStickToGround = false,
	                 int32 Priority = 100, int32 SourceId = 0,
	                 bool bSlideOnBlock = true,
	                 UCurveFloat* Curve = nullptr);

	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void MoveActorToWithSpeed(const FVector& Target, float Speed,
	                          EMovementInterpolationType Interp = EMovementInterpolationType::Linear,
	                          bool bStickToGround = false,
	                          int32 Priority = 100, int32 SourceId = 0,
	                          bool bSlideOnBlock = true,
	                          UCurveFloat* Curve = nullptr);

	// Convenience: forward direction distance
	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void MoveInFacing(float Distance, float Speed,
	                  EMovementInterpolationType Interp = EMovementInterpolationType::EaseOut,
	                  bool bStickToGround = true,
	                  int32 Priority = 100, int32 SourceId = 0,
	                  bool bSlideOnBlock = true,
	                  UCurveFloat* Curve = nullptr);

	// Convenience: move towards actor, stopping before overlap
	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void MoveTowardsActor(AActor* TargetActor, float StopDistance, float Speed,
	                      EMovementInterpolationType Interp = EMovementInterpolationType::EaseOut,
	                      bool bStickToGround = true,
	                      int32 Priority = 100, int32 SourceId = 0,
	                      bool bSlideOnBlock = true,
	                      UCurveFloat* Curve = nullptr);

	// Update current movement target/speed while moving
	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	bool UpdateMovementTarget(const FVector& NewTarget,
	                          bool bChangeSpeed, float NewSpeedOrDuration,
	                          bool bRequireSameSourceId = true,
	                          int32 SourceId = 0);

	// 지정 시간(Duration) 동안 바라보는 방향으로 이동
	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void MoveInFacingTimed(float Speed, float Duration,
						   EMovementInterpolationType Interp = EMovementInterpolationType::EaseOut,
						   bool bStickToGround = true,
						   int32 Priority = 100, int32 SourceId = 0,
						   bool bSlideOnBlock = true,
						   UCurveFloat* Curve = nullptr);

	// 타겟 추적 이동 + 최대 지속시간 경과 시 자동 중지
	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void MoveTowardsActorTimed(AActor* TargetActor, float StopDistance, float Speed, float MaxDuration,
							   EMovementInterpolationType Interp = EMovementInterpolationType::EaseOut,
							   bool bStickToGround = true,
							   int32 Priority = 100, int32 SourceId = 0,
							   bool bSlideOnBlock = true,
							   UCurveFloat* Curve = nullptr);

	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void StopMovement(EMoveFinishReason Reason = EMoveFinishReason::Canceled);

	UFUNCTION(BlueprintCallable, Category="MoveUtil")
	void CancelBySourceId(int32 InSourceId, EMoveFinishReason Reason = EMoveFinishReason::Replaced);

	UFUNCTION(BlueprintPure, Category="MoveUtil")
	bool IsMoving() const { return MovementState.IsActive(); }

	UPROPERTY(BlueprintAssignable, Category="MoveUtil")
	FOnMoveFinished OnMoveFinished;

	// Performance/Debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveUtil|Perf")
	float MinUpdateInterval = 0.f; // seconds, 0 = every tick

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveUtil|Perf")
	float LocationUpdateThreshold = 0.1f; // cm, ignore tiny moves

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoveUtil|Debug")
	bool bShowDebugPath = false;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	FMovementState MovementState;
	float LastUpdateTime = 0.f;

	void StartNewMovement(const FVector& Target, EPMMovementMode Mode, float SpeedOrDuration,
	                      EMovementInterpolationType Interp, bool bStickToGround,
	                      int32 Priority, int32 SourceId, bool bSlideOnBlock, UCurveFloat* Curve);

	static float CalculateInterpolationAlpha(float RawAlpha, EMovementInterpolationType InterpType,
	                                         const UCurveFloat* Curve);
	void ApplyGroundStick(FVector& InOutPosition) const;
	bool SafeMoveOwner(const FVector& Delta, FHitResult& OutHit);
	void DrawDebugVisuals() const;

	TMap<int32, FTimerHandle> CancelTimers; 

	void StartCancelTimer(float Seconds, int32 SourceId, EMoveFinishReason Reason = EMoveFinishReason::Timeout);
	void ClearCancelTimer(int32 SourceId);
};
