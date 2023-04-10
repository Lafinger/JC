// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "JCExperienceDefinition.generated.h"

class UJCPawnData;
class UGameFeatureAction;
class UJCExperienceActionSet;

/**
 * 
 */
UCLASS(BlueprintType, Const)
class JC_API UJCExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// List of Game Feature Plugins this experience wants to have active
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/** The default pawn class to spawn for players */
	//@TODO: Make soft?
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<const UJCPawnData> DefaultPawnData;

	// List of additional action sets to compose into this experience
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<TObjectPtr<UJCExperienceActionSet>> ActionSets;

	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};
