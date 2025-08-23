// AnimNotify_AreaMoveEx.cpp
#include "AnimNotify_Movement.h"
#include "GameFramework/Character.h"
#include "Tazan/AreaObject/Base/AreaObject.h"
#include "Tazan/AreaObject/Monster/BaseMonster.h"
#include "Tazan/AreaObject/Utility/MoveUtilComponent.h"
#include "Tazan/AreaObject/Player/Player_Kazan.h"
#include "Tazan/AreaObject/Player/Component/LockOnComponent.h"

static AActor* ResolveTarget(AAreaObject* AreaObject)
{
	if (!AreaObject) return nullptr;

	// 플레이어: 락온 우선
	if (auto* Player = Cast<APlayer_Kazan>(AreaObject))
	{
		if (auto* L = Player->GetLockOnComponent())
		{
			if (L->IsLockOnMode())
				return L->GetCurrentTarget();
		}
	}

	// 몬스터: 어그로 타겟
	if (auto* Monster = Cast<ABaseMonster>(AreaObject)) 
	{
		if (AActor* Aggro = Monster->GetAggroTarget())
			return Aggro;
	}
	return nullptr;
}

void UAnimNotify_Movement::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	AAreaObject* AreaObject = Cast<AAreaObject>(Owner);
	if (!AreaObject)
	{
		// 폴백: AAreaObject가 아니면 MoveUtil 직접 접근 시도
		if (ACharacter* Ch = Cast<ACharacter>(Owner))
		{
			if (UMoveUtilComponent* MU = Ch->FindComponentByClass<UMoveUtilComponent>())
			{
				switch (Mode)
				{
				case EAreaMoveNotifyKind::InFacing_Distance:
					{
						const FVector Fwd = Ch->GetActorForwardVector();
						MU->MoveActorToWithSpeed(Ch->GetActorLocation() + Fwd * Distance, Speed_Dist, Interp,
						                         bStickToGround, Priority, SourceId);
					}
					return;
				case EAreaMoveNotifyKind::InFacing_Duration:
					MU->MoveInFacingTimed(Speed_Time, Duration, Interp, bStickToGround, Priority, SourceId);
					return;
				case EAreaMoveNotifyKind::TowardsActor_Timed:
					// 폴백에선 락온 미지원
					return;
				}
			}
		}
		return;
	}

	switch (Mode)
	{
	case EAreaMoveNotifyKind::InFacing_Distance:
		AreaObject->MoveInFacing(Distance, Speed_Dist, Interp, bStickToGround, Priority, SourceId);
		break;

	case EAreaMoveNotifyKind::InFacing_Duration:
		AreaObject->MoveInFacingTimed(Speed_Time, Duration, Interp, bStickToGround, Priority, SourceId);
		break;

	case EAreaMoveNotifyKind::TowardsActor_Timed:
		{
			AActor* Target = ResolveTarget(AreaObject);
			if (Target)
			{
				AreaObject->MoveTowardsActorTimed(Target, StopDistance, Speed_Toward, MaxDuration, Interp,
				                                  bStickToGround, Priority, SourceId);
			}
		}
		break;
	}
}
