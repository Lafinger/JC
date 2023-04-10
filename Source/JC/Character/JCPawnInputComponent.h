// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
// #include "Containers/Array.h"
// #include "Engine/EngineTypes.h"
// #include "GameFramework/Actor.h"
// #include "GameplayAbilitySpec.h"
// #include "JC/Input/JCMappableConfigPair.h"
// #include "Templates/SubclassOf.h"
// #include "UObject/NameTypes.h"
// #include "UObject/UObjectGlobals.h"

#include "JCPawnInputComponent.generated.h"

// class UGameFrameworkComponentManager;
// class UInputComponent;
// class ULyraCameraMode;
// class ULyraInputConfig;
// class UObject;
// struct FActorInitStateChangedParams;
// struct FFrame;
struct FGameplayTag;
struct FInputActionValue;
class UInputMappingContext;

/**
 * Component that sets up input and camera handling for player controlled pawns (or bots that simulate players).
 * This depends on a PawnExtensionComponent to coordinate initialization.
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class JC_API UJCPawnInputComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:

	UJCPawnInputComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Lyra|Hero")
	static UJCPawnInputComponent* FindCharacterInputComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UJCPawnInputComponent>() : nullptr); }

	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	bool IsReadyToBindInputs() const;
	
	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	static const FName NAME_BindInputsNow;

	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	// void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	// void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	// void Input_LookStick(const FInputActionValue& InputActionValue);
	// void Input_Crouch(const FInputActionValue& InputActionValue);
	// void Input_AutoRun(const FInputActionValue& InputActionValue);

	// TSubclassOf<ULyraCameraMode> DetermineCameraMode() const;
	
	// void OnInputConfigActivated(const FLoadedMappableConfigPair& ConfigPair);
	// void OnInputConfigDeactivated(const FLoadedMappableConfigPair& ConfigPair);

protected:

	/**
	 * Input Configs that should be added to this player when initializing the input. These configs
	 * will NOT be registered with the settings because they are added at runtime. If you want the config
	 * pair to be in the settings, then add it via the GameFeatureAction_AddInputConfig
	 * 
	 * NOTE: You should only add to this if you do not have a game feature plugin accessible to you.
	 * If you do, then use the GameFeatureAction_AddInputConfig instead. 
	 */
	// UPROPERTY(EditAnywhere)
	// TArray<FMappableConfigPair> DefaultInputConfigs;
	
	// Temp
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext;
	
	// /** Camera mode set by an ability. */
	// UPROPERTY()
	// TSubclassOf<ULyraCameraMode> AbilityCameraMode;
	//
	// /** Spec handle for the last ability to set a camera mode. */
	// FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;

	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindInputs;
};