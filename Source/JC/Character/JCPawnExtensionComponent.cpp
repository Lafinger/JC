// Copyright Epic Games, Inc. All Rights Reserved.

#include "JCPawnExtensionComponent.h"

// #include "Abilities/GameplayAbilityTypes.h"
// #include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentManager.h"
// #include "Containers/Array.h"
// #include "Containers/Set.h"
// #include "Containers/UnrealString.h"
// #include "Engine/EngineBaseTypes.h"
// #include "GameFramework/Controller.h"
// #include "GameFramework/Pawn.h"
// #include "GameplayTagContainer.h"
// #include "HAL/Platform.h"
#include "Logging/LogCategory.h"
#include "Logging/LogMacros.h"
#include "JC/System/JCGameplayTags.h"
// #include "JC/JCLogChannels.h"
// #include "LyraPawnData.h"
// #include "Misc/AssertionMacros.h"
#include "Net/UnrealNetwork.h"
// #include "Templates/SharedPointer.h"
// #include "Trace/Detail/Channel.h"
// #include "UObject/UObjectBaseUtility.h"
// #include "UObject/UnrealNames.h"
// #include "UObject/WeakObjectPtr.h"
// #include "UObject/WeakObjectPtrTemplates.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(JCPawnExtensionComponent)

class FLifetimeProperty;
class UActorComponent;

const FName UJCPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UJCPawnExtensionComponent::UJCPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
	// AbilitySystemComponent = nullptr;
}

void UJCPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UJCPawnExtensionComponent, PawnData);
}

void UJCPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("LyraPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UJCPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one LyraPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register with the init state system early, this will only work if this is a game world
	RegisterInitStateFeature();
}

void UJCPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	
	// Notifies state manager that we have spawned, then try rest of default initialization
	ensure(TryToChangeInitState(FJCGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UJCPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// UninitializeAbilitySystem();
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UJCPawnExtensionComponent::SetPawnData(const UJCPawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		// UE_LOG(LogJC, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}

void UJCPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}

// void ULyraPawnExtensionComponent::InitializeAbilitySystem(ULyraAbilitySystemComponent* InASC, AActor* InOwnerActor)
// {
// 	check(InASC);
// 	check(InOwnerActor);
//
// 	if (AbilitySystemComponent == InASC)
// 	{
// 		// The ability system component hasn't changed.
// 		return;
// 	}
//
// 	if (AbilitySystemComponent)
// 	{
// 		// Clean up the old ability system component.
// 		UninitializeAbilitySystem();
// 	}
//
// 	APawn* Pawn = GetPawnChecked<APawn>();
// 	AActor* ExistingAvatar = InASC->GetAvatarActor();
//
// 	UE_LOG(LogLyra, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));
//
// 	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
// 	{
// 		UE_LOG(LogLyra, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);
//
// 		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
// 		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
// 		ensure(!ExistingAvatar->HasAuthority());
//
// 		if (ULyraPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
// 		{
// 			OtherExtensionComponent->UninitializeAbilitySystem();
// 		}
// 	}
//
// 	AbilitySystemComponent = InASC;
// 	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);
//
// 	if (ensure(PawnData))
// 	{
// 		InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
// 	}
//
// 	OnAbilitySystemInitialized.Broadcast();
// }
//
// void ULyraPawnExtensionComponent::UninitializeAbilitySystem()
// {
// 	if (!AbilitySystemComponent)
// 	{
// 		return;
// 	}
//
// 	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
// 	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
// 	{
// 		FGameplayTagContainer AbilityTypesToIgnore;
// 		AbilityTypesToIgnore.AddTag(FLyraGameplayTags::Get().Ability_Behavior_SurvivesDeath);
//
// 		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
// 		AbilitySystemComponent->ClearAbilityInput();
// 		AbilitySystemComponent->RemoveAllGameplayCues();
//
// 		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
// 		{
// 			AbilitySystemComponent->SetAvatarActor(nullptr);
// 		}
// 		else
// 		{
// 			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
// 			AbilitySystemComponent->ClearActorInfo();
// 		}
//
// 		OnAbilitySystemUninitialized.Broadcast();
// 	}
//
// 	AbilitySystemComponent = nullptr;
// }

void UJCPawnExtensionComponent::HandleControllerChanged()
{
	// if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	// {
	// 	ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
	// 	if (AbilitySystemComponent->GetOwnerActor() == nullptr)
	// 	{
	// 		UninitializeAbilitySystem();
	// 	}
	// 	else
	// 	{
	// 		AbilitySystemComponent->RefreshAbilityActorInfo();
	// 	}
	// }

	CheckDefaultInitialization();
}

void UJCPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UJCPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UJCPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

	const FJCGameplayTags& InitTags = FJCGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

bool UJCPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FJCGameplayTags& InitTags = FJCGameplayTags::Get();

	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}
	else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UJCPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == FJCGameplayTags::Get().InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void UJCPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		const FJCGameplayTags& InitTags = FJCGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}
//
// void UJCPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
// {
// 	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
// 	{
// 		OnAbilitySystemInitialized.Add(Delegate);
// 	}
//
// 	if (AbilitySystemComponent)
// 	{
// 		Delegate.Execute();
// 	}
// }
//
// void UJCPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
// {
// 	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
// 	{
// 		OnAbilitySystemUninitialized.Add(Delegate);
// 	}
// }

