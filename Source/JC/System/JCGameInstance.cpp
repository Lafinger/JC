// Copyright Epic Games, Inc. All Rights Reserved.

#include "JCGameInstance.h"

// #include "CommonSessionSubsystem.h"
#include "Components/GameFrameworkComponentManager.h"
// #include "Engine/LocalPlayer.h"
// / #include "GameplayTagContainer.h"
#include  "JC/System/JCGameplayTags.h"
// / #include "Misc/AssertionMacros.h"
// / #include "Player/LyraPlayerController.h"
// / #include "Templates/Casts.h"
// / #include "Engine/LocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(JCGameInstance)

UJCGameInstance::UJCGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UJCGameInstance::Init()
{
	Super::Init();

	// Register our custom init states
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		const FJCGameplayTags& GameplayTags = FJCGameplayTags::Get();

		ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_DataAvailable);
		ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
	}
}

void UJCGameInstance::Shutdown()
{
	Super::Shutdown();
}

APlayerController* UJCGameInstance::GetPrimaryPlayerController() const
{
	return Cast<APlayerController>(Super::GetPrimaryPlayerController(false));
}

// bool UJCGameInstance::CanJoinRequestedSession() const
// {
// 	// Temporary first pass:  Always return true
// 	// This will be fleshed out to check the player's state
// 	if (!Super::CanJoinRequestedSession())
// 	{
// 		return false;
// 	}
// 	return true;
// }
