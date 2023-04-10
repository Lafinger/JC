// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "JCPawnData.generated.h"

class UJCInputConfig;

/**
 * 
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "JC Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class JC_API UJCPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UJCPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from ALyraPawn or ALyraCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "JC|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "JC|Input")
	TObjectPtr<UJCInputConfig> InputConfig;

	//// Default camera mode used by player controlled pawns.
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Camera")
	//TSubclassOf<ULyraCameraMode> DefaultCameraMode;

	//// Ability sets to grant to this pawn's ability system.
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	//TArray<TObjectPtr<ULyraAbilitySet>> AbilitySets;

	//// What mapping of ability tags to use for actions taking by this pawn
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	//TObjectPtr<ULyraAbilityTagRelationshipMapping> TagRelationshipMapping;
	
};
