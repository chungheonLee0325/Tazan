#pragma once

#include <Engine/DataTable.h>
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "KazanGameType.generated.h"


// Enum
// 언리얼 리플렉션 시스템과 통합하기 위해 UENUM() 매크로를 사용
UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EConditionType : uint8
{
	None = 0, // 0b0000
	Dead = 1 << 0, // 0b0001
	Invincible = 1 << 1, // 0b0010
	Damaged = 1 << 2, // 0b0100
	StrongDamaged = 1 << 3,
	KnockDown = 1 << 4,
};

ENUM_CLASS_FLAGS(EConditionType);

UENUM(BlueprintType)
enum class EAreaObjectType : uint8
{
	None UMETA(DisplayName = "None"), // 블루프린트에서 표시될 이름
	Player UMETA(DisplayName = "Player"),
	Enemy UMETA(DisplayName = "Enemy"),
};

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	None UMETA(DisplayName = "None"), // 블루프린트에서 표시될 이름
	Monster UMETA(DisplayName = "Monster"),
	NamedMonster UMETA(DisplayName = "NamedMonster"),
	Boss UMETA(DisplayName = "Boss"),
};

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

UENUM(BlueprintType)
enum class EHitDetectionType : uint8
{
	Line        UMETA(DisplayName = "Line Trace"),
	Sphere      UMETA(DisplayName = "Sphere Trace"),
	Capsule     UMETA(DisplayName = "Capsule Trace"),
	Box         UMETA(DisplayName = "Box Trace")
};


// Struct
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
};


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


USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthDamageAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaDamageAmount = 0.0f;

	// 강인도 : 0 ~ 5 까지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PoiseLevel = 0; 

	// 경직 정도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConditionType ConditionType = EConditionType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitBoxData HitBoxData;
};

USTRUCT(BlueprintType)
struct FSkill_Data : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SkillID = 0;

	// 소모 자원(현재는 스태미너 고정 추후 체력등 확장시 enum 추가될듯)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Cost = 0;

	// 스킬 사정거리 (AI 용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkillRange = 0.0f;

	// 데미지 관련 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackData AttackData;

	// Todo : AnimMontage & Sound & Cast/Hit FX 관련 항목 추가?
};


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
