// Copyright Epic Games, Inc. All Rights Reserved.

#include "JCPlayerState.h"

// #include "AbilitySystem/Attributes/LyraCombatSet.h"
// #include "AbilitySystem/Attributes/LyraHealthSet.h"
// #include "AbilitySystem/LyraAbilitySet.h"
// #include "AbilitySystem/LyraAbilitySystemComponent.h"
// #include "AbilitySystemComponent.h"
#include "JC/System/Experience/JCPawnData.h"
#include "JC/Character/JCPawnExtensionComponent.h"
// #include "Components/GameFrameworkComponentManager.h"
// #include "Containers/Array.h"
// #include "Containers/UnrealString.h"
// #include "CoreTypes.h"
// #include "Delegates/Delegate.h"
// #include "Engine/EngineBaseTypes.h"
// #include "Engine/EngineTypes.h"
// #include "Engine/World.h"
// #include "GameFramework/GameStateBase.h"
// #include "GameFramework/GameplayMessageSubsystem.h"
// #include "GameFramework/Pawn.h"
#include "JC/GameMode/JCExperienceManagerComponent.h"
// //@TODO: Would like to isolate this a bit better to get the pawn data in here without this having to know about other stuff
#include "JC/GameMode/JCGameMode.h"
// #include "GameplayTagContainer.h"
// #include "Logging/LogCategory.h"
// #include "Logging/LogMacros.h"
#include "JC/JCLogChannels.h"
// #include "LyraPlayerController.h"
// #include "Misc/AssertionMacros.h"
// #include "Net/Core/PushModel/PushModel.h"
// #include "Net/UnrealNetwork.h"
// #include "Trace/Detail/Channel.h"
// #include "UObject/NameTypes.h"
// #include "UObject/UObjectBaseUtility.h"



#include UE_INLINE_GENERATED_CPP_BY_NAME(JCPlayerState)

class AController;
class APlayerState;
class FLifetimeProperty;

// const FName ALyraPlayerState::NAME_LyraAbilityReady("LyraAbilitiesReady");

AJCPlayerState::AJCPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	// , MyPlayerConnectionType(ELyraPlayerConnectionType::Player)
{
	// AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<ULyraAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	// AbilitySystemComponent->SetIsReplicated(true);
	// AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	//
	// CreateDefaultSubobject<ULyraHealthSet>(TEXT("HealthSet"));
	// CreateDefaultSubobject<ULyraCombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;

	// MyTeamID = FGenericTeamId::NoTeam;
	// MySquadID = INDEX_NONE;
}

void AJCPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AJCPlayerState::Reset()
{
	Super::Reset();
}

void AJCPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (UJCPawnExtensionComponent* PawnExtComp = UJCPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void AJCPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
}

void AJCPlayerState::OnExperienceLoaded(const UJCExperienceDefinition* CurrentExperience)
{
	if (AJCGameMode* JCGameMode = GetWorld()->GetAuthGameMode<AJCGameMode>())
	{
		if (const UJCPawnData* NewPawnData = JCGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogJC, Error, TEXT("ALyraPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}

// void AJCPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	FDoRepLifetimeParams SharedParams;
// 	SharedParams.bIsPushBased = true;
//
// 	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
// 	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams)
// 	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamID, SharedParams);
// 	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MySquadID, SharedParams);
//
// 	DOREPLIFETIME(ThisClass, StatTags);
// }

AController* AJCPlayerState::GetJCPlayerController() const
{
	return Cast<AController>(GetOwner());
}

void AJCPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// check(AbilitySystemComponent);
	// AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	if (GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UJCExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UJCExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnJCExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void AJCPlayerState::SetPawnData(TObjectPtr<const UJCPawnData> InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		// UE_LOG(LogJC, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	// MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	// for (const ULyraAbilitySet* AbilitySet : PawnData->AbilitySets)
	// {
	// 	if (AbilitySet)
	// 	{
	// 		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
	// 	}
	// }

	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_LyraAbilityReady);
	
	// ForceNetUpdate();
}

