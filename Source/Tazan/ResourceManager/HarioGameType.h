#pragma once

#include <Engine/DataTable.h>
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "HarioGameType.generated.h"


// Enum
// 언리얼 리플렉션 시스템과 통합하기 위해 UENUM() 매크로를 사용
UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EConditionType : uint8
{
	None        = 0,        // 0b0000
	Dead        = 1 << 0,   // 0b0001
	Invincible  = 1 << 1,   // 0b0010
	Hide        = 1 << 2    // 0b0100
};
ENUM_CLASS_FLAGS(EConditionType);

UENUM(BlueprintType)
enum class EAreaObjectType : uint8 {
	None        UMETA(DisplayName = "None"),       // 블루프린트에서 표시될 이름
	Player     UMETA(DisplayName = "Player"),
	Enemy     UMETA(DisplayName = "Enemy"),
};

UENUM(BlueprintType)
enum class EEnemyType : uint8 {
	None        UMETA(DisplayName = "None"),       // 블루프린트에서 표시될 이름
	Monster     UMETA(DisplayName = "Monster"),
	NamedMonster     UMETA(DisplayName = "NamedMonster"),
	Boss     UMETA(DisplayName = "Boss"),
};

UENUM(BlueprintType)
enum class EAiStateType : uint8 {
	None        UMETA(DisplayName = "None"),       
	Idle     UMETA(DisplayName = "Idle"),
	Wait     UMETA(DisplayName = "Wait"),
	Attack     UMETA(DisplayName = "Attack"),
	SelectSkill UMETA(DisplayName = "SelectSkill"),
	Chase     UMETA(DisplayName = "Chase"),
	Return    UMETA(DisplayName = "Return"),
};

// Struct
USTRUCT(BlueprintType)
struct FAreaObjectData:public FTableRowBase
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
struct FSoundData:public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	int SoundID = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	USoundBase *Sound = nullptr;
};