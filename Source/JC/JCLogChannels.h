// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "Logging/LogMacros.h"

class UObject;

JC_API DECLARE_LOG_CATEGORY_EXTERN(LogJC, Log, All);
JC_API DECLARE_LOG_CATEGORY_EXTERN(LogJCExperience, Log, All);
// JC_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraAbilitySystem, Log, All);
// JC_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraTeams, Log, All);

JC_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
