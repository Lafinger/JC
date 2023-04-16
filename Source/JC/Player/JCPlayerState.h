// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// #include "AbilitySystemInterface.h"
// #include "GenericTeamAgentInterface.h"
// #include "HAL/Platform.h"
// #include "Messages/LyraVerbMessage.h"
#include "ModularPlayerState.h"
// #include "System/GameplayTagStack.h"
// #include "Teams/LyraTeamAgentInterface.h"
// #include "Templates/Casts.h"
// #include "UObject/UObjectGlobals.h"
#include "JC/System/Experience/JCPawnData.h"

#include "JCPlayerState.generated.h"

class AController;
// class ALyraPlayerController;
// class APlayerState;
// class FName;
// class UAbilitySystemComponent;
// class ULyraAbilitySystemComponent;
class UJCExperienceDefinition;
class UJCPawnData;


/**
 * ALyraPlayerState
 *
 *	Base player state class used by this project.
 */
UCLASS(Config = Game)
class JC_API AJCPlayerState : public AModularPlayerState
/*, public IAbilitySystemInterface, public ILyraTeamAgentInterface*/
{
	GENERATED_BODY()

public:
	AJCPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	// virtual void OnDeactivated() override;
	// virtual void OnReactivated() override;
	//~End of APlayerState interface

	// UFUNCTION(BlueprintCallable, Category = "Lyra|PlayerState")
	// ALyraPlayerController* GetLyraPlayerController() const;
	UFUNCTION(BlueprintCallable, Category = "JC|PlayerState") // Temp
	AController* GetJCPlayerController() const;

	// UFUNCTION(BlueprintCallable, Category = "Lyra|PlayerState")
	// ULyraAbilitySystemComponent* GetLyraAbilitySystemComponent() const { return AbilitySystemComponent; }
	// virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	// void SetPawnData(const UJCPawnData* InPawnData);
	void SetPawnData(TObjectPtr<const UJCPawnData> InPawnData); // Temp
	
private:
	void OnExperienceLoaded(const UJCExperienceDefinition* CurrentExperience);

protected:
	TObjectPtr<const UJCPawnData> PawnData;
};
