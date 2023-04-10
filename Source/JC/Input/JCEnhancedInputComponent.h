// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "EnhancedInputComponent.h"

// #include "Containers/Array.h"
// #include "GameplayTagContainer.h"
// #include "HAL/Platform.h"
// #include "InputTriggers.h"
// #include "Misc/AssertionMacros.h"
// #include "UObject/UObjectGlobals.h"
#include "JC/System/Experience/JCInputConfig.h"

#include "JCEnhancedInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
// class UObject;


/**
 * ULyraInputComponent
 *
 *	Component used to manage input mappings and bindings using an input config data asset.
 */
UCLASS(Config = Input)
class UJCEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	void AddInputMappings(const UJCInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UJCInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UJCInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	// template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	// void BindAbilityActions(const ULyraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
};


template<class UserClass, typename FuncType>
void UJCEnhancedInputComponent::BindNativeAction(const UJCInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}
//
// template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
// void UJCEnhancedInputComponent::BindAbilityActions(const ULyraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
// {
// 	check(InputConfig);
//
// 	for (const FLyraInputAction& Action : InputConfig->AbilityInputActions)
// 	{
// 		if (Action.InputAction && Action.InputTag.IsValid())
// 		{
// 			if (PressedFunc)
// 			{
// 				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
// 			}
//
// 			if (ReleasedFunc)
// 			{
// 				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
// 			}
// 		}
// 	}
// }
