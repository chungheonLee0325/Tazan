// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tazan/ResourceManager/KazanGameType.h"
#include "BaseAiFSM.generated.h"

class ABaseMonster;
class UBaseAiState;

UCLASS(Abstract,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TAZAN_API UBaseAiFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseAiFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void AddState(EAiStateType StateType,UBaseAiState* State);
	
	void ChangeState(EAiStateType StateType);

	bool IsExistState(EAiStateType StateType) const;

	virtual void UpdateState(float dt);

	virtual void InitStatePool() PURE_VIRTUAL(UBaseAiFSM::InitStatePool,);

	void StopFSM() {m_CurrentState = nullptr;}

public:
	template<typename T>
	static T* CreateState(UObject* Outer, ABaseMonster* Owner, EAiStateType NextState)
	{
		static_assert(std::is_base_of_v<UBaseAiState, T>, "T must derive from UAiState");

		T* State = NewObject<T>(Outer, T::StaticClass());
		State->InitState();
		State->SetOwner(Owner);
		State->SetNextState(NextState);
		return State;
	}

	
	
protected:
	UPROPERTY()
	ABaseMonster* m_Owner = nullptr;

private:
	
UPROPERTY()
	TMap<EAiStateType, UBaseAiState*> m_AiStates;
	
UPROPERTY()
	UBaseAiState* m_CurrentState = nullptr;
UPROPERTY()
	UBaseAiState* m_PreviousState = nullptr;
};
