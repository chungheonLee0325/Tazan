#include "CameraShakeNotifyState.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

void UCameraShakeNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp || !MeshComp->GetWorld()) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(MeshComp->GetWorld(), 0);
	if (PC && CameraShakeClass)
	{
		APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
		UCameraShakeBase* ShakeInstance = CameraManager->StartCameraShake(CameraShakeClass, ShakeScale);
		if (ShakeInstance)
		{
		}
	}
}

void UCameraShakeNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MeshComp->GetWorld()) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(MeshComp->GetWorld(), 0);
	if (PC)
	{
		// Notify 종료 시, 동일한 셰이크 클래스로 실행중인 모든 인스턴스를 중지
		PC->PlayerCameraManager->StopAllInstancesOfCameraShake(CameraShakeClass);
	}
}
