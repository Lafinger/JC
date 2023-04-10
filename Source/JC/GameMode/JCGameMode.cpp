// Fill out your copyright notice in the Description page of Project Settings.


#include "JCGameMode.h"

#include "JCExperienceManagerComponent.h"
#include "JCGameState.h"
#include "JC/Character/JCCharacter.h"
#include "JC/System/JCAssetManager.h"
#include "JC/System/Experience/JCExperienceDefinition.h"
#include "JC/JCLogChannels.h"
#include "JC/Character/JCPawnExtensionComponent.h"
#include "JC/Player/JCPlayerState.h"

#include "Kismet/GameplayStatics.h"

AJCGameMode::AJCGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GameStateClass = AJCGameState::StaticClass();
	DefaultPawnClass = AJCCharacter::StaticClass();
	PlayerControllerClass = APlayerController::StaticClass();
	PlayerStateClass = AJCPlayerState::StaticClass();
}

const UJCPawnData* AJCGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const AJCPlayerState* LyraPS = InController->GetPlayerState<AJCPlayerState>())
		{
			if (const UJCPawnData* PawnData = LyraPS->GetPawnData<UJCPawnData>())
			{
				return PawnData;
			}
		}
	}

	// If not, fall back to the the default for the current experience
	check(GameState);
	UJCExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UJCExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		const UJCExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		// Experience is loaded and there's still no pawn data, fall back to the default for now
		// return UJCAssetManager::Get().GetDefaultPawnData();
		return nullptr;
	}

	// Experience not loaded yet, so there is no pawn data to be had
	return nullptr;
}

void AJCGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	//@TODO: Eventually only do this for PIE/auto
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

UClass* AJCGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UJCPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AJCGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UJCPawnExtensionComponent* PawnExtComp = UJCPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UJCPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogJC, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogJC, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogJC, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

bool AJCGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

void AJCGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Delay starting new players until the experience has been loaded
	// (players who log in prior to that will be started by OnExperienceLoaded)
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* AJCGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// if (ULyraPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<ULyraPlayerSpawningManagerComponent>())
 //    {
 //    	return PlayerSpawningComponent->ChoosePlayerStart(Player);
 //    }
    
    return Super::ChoosePlayerStart_Implementation(Player);
}

void AJCGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	// if (ULyraPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<ULyraPlayerSpawningManagerComponent>())
 //    {
 //    	PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
 //    }

    Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

bool AJCGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

void AJCGameMode::InitGameState()
{
	Super::InitGameState();

	// Listen for the experience load to complete	
	UJCExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UJCExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnJCExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

bool AJCGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{	
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot version of Super::PlayerCanRestart_Implementation
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	// if (ULyraPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<ULyraPlayerSpawningManagerComponent>())
	// {
	// 	return PlayerSpawningComponent->ControllerCanRestart(Controller);
	// }

	return true;
}

void AJCGameMode::OnExperienceLoaded(const UJCExperienceDefinition* CurrentExperience)
{
	// Spawn any players that are already attached
	//@TODO: Here we're handling only *player* controllers, but in GetDefaultPawnClassForController_Implementation we skipped all controllers
	// GetDefaultPawnClassForController_Implementation might only be getting called for players anyways
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

bool AJCGameMode::IsExperienceLoaded() const
{
	check(GameState);
	UJCExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UJCExperienceManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

void AJCGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	// Precedence order (highest wins)
	//  - Matchmaking assignment (if present)
	//  - URL Options override
	//  - Developer Settings (PIE only)
	//  - Command Line override
	//  - World Settings
	//  - Default experience

	UWorld* World = GetWorld();

	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UJCExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}
	
	if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	{
		// ExperienceId = GetDefault<ULyraDeveloperSettings>()->ExperienceOverride;
		// ExperienceIdSource = TEXT("DeveloperSettings");
	}
	
	// see if the command line wants to set the experience
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			ExperienceIdSource = TEXT("CommandLine");
		}
	}
	
	// see if the world settings has a default experience
	if (!ExperienceId.IsValid())
	{
		// if (ALyraWorldSettings* TypedWorldSettings = Cast<ALyraWorldSettings>(GetWorldSettings()))
		// {
		// 	ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
		// 	ExperienceIdSource = TEXT("WorldSettings");
		// }
	}
	
	UJCAssetManager& AssetManager = UJCAssetManager::Get();
	
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogJCExperience, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}

	// Final fallback to the default experience
	if (!ExperienceId.IsValid())
	{
		//@TODO: Pull this from a config setting or something
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("JCExperienceDefinition"), FName("BP_JCDefaultExperience"));
		ExperienceIdSource = TEXT("Default");
	}

	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

void AJCGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
#if WITH_SERVER_CODE
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogJCExperience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		UJCExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UJCExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->ServerSetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogJCExperience, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
#endif
}
