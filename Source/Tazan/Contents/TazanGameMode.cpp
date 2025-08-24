// Fill out your copyright notice in the Description page of Project Settings.


#include "TazanGameMode.h"

#include "TazanGameInstance.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ATazanGameMode::ATazanGameMode()
{
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMAudioComponennt"));
	AudioComponent->bAutoDestroy = false;
	AudioComponent->bAutoManageAttachment = false;
	SoundDataMap.Empty();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Script/Engine.Blueprint'/Game/_BluePrints/AreaObject/Player/BP_Player.BP_Player_C'"));
	if (PlayerPawnBPClass.Succeeded())
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<AController> PlayerControllerBPClass(TEXT("/Script/Engine.Blueprint'/Game/_BluePrints/AreaObject/Player/BP_PlayerController.BP_PlayerController_C'"));
	if (PlayerControllerBPClass.Succeeded())
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}

void ATazanGameMode::BeginPlay()
{
	Super::BeginPlay();

	//PlayerDied 델리게이트를 게임 모드의 PlayerDied 함수에 바인딩.
	if (!OnPlayerDied.IsBound())
	{
		OnPlayerDied.AddDynamic(this, &ATazanGameMode::PlayerDied);
	}
	auto gameInstance = Cast<UTazanGameInstance>(GetGameInstance());
	if (nullptr == gameInstance)
	{
		return;
	}
	SoundDataMap = gameInstance->SoundDataMap;

	//PlayBGM(BGMID, true);
}


void ATazanGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}

void ATazanGameMode::PlayerDied(ACharacter* Character)
{
	//캐릭터의 플레이어 컨트롤러에 대한 레퍼런스 구하기
	AController* CharacterController = Character->GetController();
	RestartPlayer(CharacterController);
}

void ATazanGameMode::PlayGlobalSound(int SoundID)
{
	auto data = SoundDataMap.Find(SoundID);
	if (data != nullptr)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), *data);
	}
}

void ATazanGameMode::PlayPositionalSound(int SoundID, FVector Position)
{
	auto data = SoundDataMap.Find(SoundID);
	if (data != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), *data, Position);
	}
}

void ATazanGameMode::PlayBGM(int SoundID, bool bLoop)
{
	auto data = SoundDataMap.Find(SoundID);
	// 이전 BGM 정지
	StopBGM();

	// 새로운 BGM 설정
	if (data)
	{
		CurrentBGM = *data;
		AudioComponent->SetSound(CurrentBGM);
		AudioComponent->bAlwaysPlay = bLoop;
		AudioComponent->Play();
	}
}

void ATazanGameMode::PlayBGMBySoundBase(USoundBase* SoundBase, bool bLoop)
{
	// 이전 BGM 정지
	StopBGM();

	// 새로운 BGM 설정
	if (SoundBase)
	{
		CurrentBGM = SoundBase;
		AudioComponent->SetSound(CurrentBGM);
		AudioComponent->bAlwaysPlay = bLoop;
		AudioComponent->Play();
	}
}

void ATazanGameMode::StopBGM()
{
	// 현재 재생 중인 BGM 정지
	if (AudioComponent)
	{
		AudioComponent->Stop();
		CurrentBGM = nullptr;
	}
}

void ATazanGameMode::SwitchBGMTemporary(int SoundID, float LifeTime)
{
	PreviousBGM = CurrentBGM;
	PlayBGM(SoundID, false);

	TWeakObjectPtr<ATazanGameMode> WeakThis = this;
	GetWorld()->GetTimerManager().SetTimer(SwitchBGMHandle, [WeakThis]()
	{
		if (auto StrongThis = WeakThis.Get())
		{
			StrongThis->PlayBGMBySoundBase(StrongThis->PreviousBGM, true);
		}
	}, LifeTime, false);
}

void ATazanGameMode::SetBGMVolume(float Volume)
{
	// BGM 볼륨 조절 (0.0f ~ 1.0f)
	if (AudioComponent)
	{
		AudioComponent->SetVolumeMultiplier(FMath::Clamp(Volume, 0.0f, 1.0f));
	}
}
