// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "Engine/EngineTypes.h"
#include "ModularGameState.h"
#include "UObject/UObjectGlobals.h"

#include "JCGameState.generated.h"

class UJCExperienceManagerComponent;


/**
 * AJCGameState
 *
 *	The base game state class used by this project.
 */
UCLASS(Config = Game)
class JC_API AJCGameState : public AModularGameStateBase
{
	GENERATED_BODY()

public:
	AJCGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY()
	TObjectPtr<UJCExperienceManagerComponent> ExperienceManagerComponent;

};
