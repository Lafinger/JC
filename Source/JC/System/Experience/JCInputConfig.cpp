// Fill out your copyright notice in the Description page of Project Settings.


#include "JCInputConfig.h"

const UInputAction* UJCInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FJCInputAction& TaggedInputAction : NativeInputActions)
	{
		if (TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
		{
			return TaggedInputAction.InputAction;
		}
	}

	return nullptr;
}
