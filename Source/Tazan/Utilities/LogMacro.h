#pragma once

#include "CoreMinimal.h"

#define LOG_SCREEN_MY(Time, Color, Format, ...) \
if (GEngine) GEngine->AddOnScreenDebugMessage(-1, Time, Color, FString::Printf(TEXT(Format), ##__VA_ARGS__));

#define LOG_SCREEN(Format, ...) \
LOG_SCREEN_MY(2.0f, FColor::Green, Format,  ##__VA_ARGS__)
