// AnimNotifyState_AreaMoveWindow.cpp
#include "AnimNotifyState_MovementWindow.h"

#include "Components/CapsuleComponent.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Utility/MoveUtilComponent.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
#include "Tazan/AreaObject/Player/Component/LockOnComponent.h"

static AActor* ResolveTarget(AAreaObject* Area, bool bUseLockOn)
{
	if (!bUseLockOn) return nullptr;
	// 1) 플레이어: 락온 우선
	if (APlayer_Kazan* Player = Cast<APlayer_Kazan>(Area))
	{
		if (auto* L = Player->GetLockOnComponent())
		{
			if (L->IsLockOnMode()) return L->GetCurrentTarget();
		}
	}
	// 2) 몬스터: 어그로 타겟 우선
	if (auto* Monster = Cast<ABaseMonster>(Area))
		if (AActor* Aggro = Monster->GetAggroTarget())
			return Aggro;
	return nullptr;
}

static FVector ComputeTowardTargetPos(AActor* Owner, AActor* Target, float StopDistance)
{
	if (!Owner || !Target) return Owner ? Owner->GetActorLocation() : FVector::ZeroVector;

	FVector OwnerLoc = Owner->GetActorLocation();
	FVector TargetLoc = Target->GetActorLocation();
	FVector Dir = TargetLoc - OwnerLoc;
	Dir.Z = 0.f;
	Dir = Dir.GetSafeNormal();

	float ExtraRadius = 0.f;
	if (const ACharacter* Ch = Cast<ACharacter>(Owner))
		if (const UCapsuleComponent* Cap = Ch->GetCapsuleComponent())
			ExtraRadius += Cap->GetScaledCapsuleRadius();
	if (const ACharacter* ChT = Cast<ACharacter>(Target))
		if (const UCapsuleComponent* CapT = ChT->GetCapsuleComponent())
			ExtraRadius += CapT->GetScaledCapsuleRadius();

	const float Stop = FMath::Max(StopDistance, 0.f) + ExtraRadius;
	const float Dist = FVector::Dist2D(TargetLoc, OwnerLoc);
	const float Travel = FMath::Max(Dist - Stop, 0.f);

	return OwnerLoc + Dir * Travel;
}

void UAnimNotifyState_MovementWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (!bStartOnBegin || !MeshComp) return;
	AAreaObject* AreaObject = MeshComp ? Cast<AAreaObject>(MeshComp->GetOwner()) : nullptr;
	if (!AreaObject) return;

	switch (Mode)
	{
	case EAreaMoveNotifyKind::InFacing_Distance:
		AreaObject->MoveInFacing(Distance, Speed_Toward, Interp, bStickToGround, Priority, SourceId);
		break;

	case EAreaMoveNotifyKind::InFacing_Duration:
		AreaObject->MoveInFacingTimed(Speed_Time, Duration, Interp, bStickToGround, Priority, SourceId);
		break;

	case EAreaMoveNotifyKind::TowardsActor_Timed:
		{
			if (AActor* Target = ResolveTarget(AreaObject, bUseLockOnTarget))
			{
				// 상태 길이(TotalDuration)를 최대 지속으로 쓰고 싶다면 여기서 MaxDuration=TotalDuration 전달도 가능
				AreaObject->MoveTowardsActorTimed(Target, StopDistance, Speed_Toward, TotalDuration, Interp, bStickToGround,
				                            Priority, SourceId);
			}
		}
		break;
	}
}

void UAnimNotifyState_MovementWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (!MeshComp) return;
	AAreaObject* AreaObject = Cast<AAreaObject>(MeshComp->GetOwner());
	if (!AreaObject) return;

	if (Mode == EAreaMoveNotifyKind::TowardsActor_Timed && bUpdateTargetEveryTick)
	{
		if (AActor* Target = ResolveTarget(AreaObject, bUseLockOnTarget))
		{
			const FVector NewTarget = ComputeTowardTargetPos(AreaObject, Target, StopDistance);
			// 속도/시간은 유지한 채 목표점만 갱신
			AreaObject->UpdateMoveToTarget(NewTarget, /*bChangeSpeed=*/false, /*NewSpeedOrDuration=*/0.f,
			                         /*bRequireSameSourceId=*/true, /*SourceId=*/SourceId);
		}
	}
}

void UAnimNotifyState_MovementWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (!bStopOnEnd || !MeshComp) return;
	if (AAreaObject* AreaObject = Cast<AAreaObject>(MeshComp->GetOwner()))
	{
		// 해당 SourceId만 멈추고 싶다면 MoveUtil에 CancelBySourceId를 직접 노출/호출하도록 확장해도 됨
		AreaObject->StopMove(EMoveFinishReason::Canceled);
	}
}