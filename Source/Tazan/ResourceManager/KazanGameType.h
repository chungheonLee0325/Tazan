#pragma once

#include <Engine/DataTable.h>
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "KazanGameType.generated.h"


// Enum
// 언리얼 리플렉션 시스템과 통합하기 위해 UENUM() 매크로를 사용

class UBaseSkill;
// ConditionBits - 비트마스크를 활용한 죽음, 무적 
UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EConditionBitsType : uint8
{
	None = 0, // 0b0000
	Dead = 1 << 0, // 0b0001
	Invincible = 1 << 1, // 0b0010
};

ENUM_CLASS_FLAGS(EConditionBitsType);

// 체력Bar, 공격 및 피격 판정 확인 등 다양한 상황에서 사용
UENUM(BlueprintType)
enum class EAreaObjectType : uint8
{
	None UMETA(DisplayName = "None"), // 블루프린트에서 표시될 이름
	Player UMETA(DisplayName = "Player"),
	Enemy UMETA(DisplayName = "Enemy"),
};

// 몬스터 종류 구별용 Type - 체력Bar 등 다양한 상황에서 사용 가능
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	None UMETA(DisplayName = "None"), // 블루프린트에서 표시될 이름
	Monster UMETA(DisplayName = "Monster"),
	NamedMonster UMETA(DisplayName = "NamedMonster"),
	Boss UMETA(DisplayName = "Boss"),
};

// AiFSM을 위한 Enum Type
UENUM(BlueprintType)
enum class EAiStateType : uint8
{
	None UMETA(DisplayName = "None"),
	Idle UMETA(DisplayName = "Idle"),
	Wait UMETA(DisplayName = "Wait"),
	Attack UMETA(DisplayName = "Attack"),
	SelectSkill UMETA(DisplayName = "SelectSkill"),
	Chase UMETA(DisplayName = "Chase"),
	Return UMETA(DisplayName = "Return"),
};

// 공격시 Trace System에서 사용 - EnableCollisionNotifyState 참조
UENUM(BlueprintType)
enum class EHitDetectionType : uint8
{
	Line UMETA(DisplayName = "Line Trace"),
	Sphere UMETA(DisplayName = "Sphere Trace"),
	Capsule UMETA(DisplayName = "Capsule Trace"),
	Box UMETA(DisplayName = "Box Trace")
};

// 경직 타입 정의
UENUM(BlueprintType)
enum class EStaggerType : uint8
{
	None,
	Weak, // 약한 데미지 경직
	Normal, // 일반 데미지 경직
	Strong, // 강한 데미지 경직
	AirBone // 넘어짐
};

// Struct
// AreaObject 데이터 테이블용 구조체
USTRUCT(BlueprintType)
struct FAreaObjectData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	int AreaObjectID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	FName Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	EAreaObjectType AreaObjectType = EAreaObjectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	EEnemyType EnemyType = EEnemyType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	float HPMax = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	int BasePoise = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	TMap<EStaggerType,UAnimMontage*> Stagger_AnimMontages;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	TSet<int> SkillList;
};

// HitBox 동적으로 생성하기 위한 구조체 정보, FAttackData 멤버 변수
USTRUCT(BlueprintType)
struct FHitBoxData
{
	GENERATED_BODY()

	// 히트 박스 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHitDetectionType DetectionType = EHitDetectionType::Line;

	// 소켓 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StartSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EndSocketName;

	// 크기 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "DetectionType != EHitDetectionType::Line"))
	float Radius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "DetectionType == EHitDetectionType::Capsule"))
	float HalfHeight = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "DetectionType == EHitDetectionType::Box"))
	FVector BoxExtent = FVector(15.0f);

	// 소켓이 있는 메시 컴포넌트 지정
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MeshComponentTag = NAME_None;
};

// FSkill의 멤버 변수, 전투에서 공격에 관련된 데이터 담당
USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_USTRUCT_BODY()

	// 체력 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthDamageAmountMin = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthDamageAmountMax = 0.0f;

	// 스테미나 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaDamageAmount = 0.0f;

	// 강인도 공격 Level : 0 ~ 15 까지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PoiseAttackAmount = 1;

	// 강인도 비교에 의해 경직 적용시 Stagger(경직) 타입 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStaggerType StaggerType = EStaggerType::None;

	// 넉백 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KnockBackForce = 50.0f;

	// 공격 히트 박스 정보 구조체
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitBoxData HitBoxData;
};

// m_SkillData 테이블 정보, 데미지 정보등 관리
USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SkillID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UBaseSkill> SkillClass = nullptr;
	
	// 소모 자원(현재는 스태미너 고정 추후 체력등 확장시 enum 추가될듯)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Cost = 0;

	// 스킬 사정거리 (AI 용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CastRange = 0.0f;

	// 스킬 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CoolTime = 0.0f;
	
	// 사용 스킬의 애님 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage = nullptr;

	// 스킬 시전중 시전자의 강인도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AnimationPoiseBonusValue = 1;

	// 데미지 관련 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackData AttackData;

	// 다음 스킬 ID (플레이어 및 몬스터 콤보 어택)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NextSkillID = 0;
	// Todo : AnimMontage & Sound & Cast/Hit FX 관련 항목 추가?
};

// SoundDataTable 데이터, GameMode에서 관리
// Why Not GameInstance? AudioComponent가질수없음 -> BGM 관리 불가
USTRUCT(BlueprintType)
struct FSoundData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	int SoundID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	USoundBase* Sound = nullptr;
};
