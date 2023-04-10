// Copyright Epic Games, Inc. All Rights Reserved.

#include "JCPawnInputComponent.h"
#include "JC/JCLogChannels.h"
// #include "GameFramework/Pawn.h"
// #include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
// #include "Player/LyraPlayerController.h"
#include "JC/Player/JCPlayerState.h"
// #include "Player/LyraLocalPlayer.h"
#include "JC/Character/JCPawnExtensionComponent.h"
#include "JC/System/Experience/JCPawnData.h"
// #include "Character/LyraCharacter.h"
// #include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "JC/System/Experience/JCInputConfig.h"
#include "JC/Input/JCEnhancedInputComponent.h"
// #include "Camera/LyraCameraComponent.h"
#include "JC/System/JCGameplayTags.h"
// #include "Engine/LocalPlayer.h"
#include "Components/GameFrameworkComponentManager.h"
// #include "Settings/LyraSettingsLocal.h"
// #include "System/LyraAssetManager.h"
// #include "PlayerMappableInputConfig.h"
// #include "Camera/LyraCameraMode.h"
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(JCPawnInputComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

// namespace LyraHero
// {
// 	static const float LookYawRate = 300.0f;
// 	static const float LookPitchRate = 165.0f;
// };

const FName UJCPawnInputComponent::NAME_BindInputsNow("BindInputsNow");
const FName UJCPawnInputComponent::NAME_ActorFeatureName("Hero");

UJCPawnInputComponent::UJCPawnInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
}

void UJCPawnInputComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogJC, Error, TEXT("[UJCPawnInputComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("JCPawnInputComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName InputComponentMessageLogName = TEXT("JCPawnInputComponent");
			
			FMessageLog(InputComponentMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(InputComponentMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UJCPawnInputComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FJCGameplayTags& InitTags = FJCGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<AJCPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			// ALyraPlayerController* LyraPC = GetController<ALyraPlayerController>();
			APlayerController* PC = GetController<APlayerController>(); // Temp

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !PC || !PC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// Wait for player state and extension component
		AJCPlayerState* JCPS = GetPlayerState<AJCPlayerState>();
		
		// Debug FoundState == InitTags.InitState_DataInitialized
		FGameplayTag FoundState = Manager->GetInitStateForFeature(Pawn, UJCPawnExtensionComponent::NAME_ActorFeatureName);
		bool bIsReached = Manager->HasFeatureReachedInitState(Pawn, UJCPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
		// Debug FoundState == InitTags.InitState_DataInitialized
		
		return JCPS && Manager->HasFeatureReachedInitState(Pawn, UJCPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}
	else if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UJCPawnInputComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FJCGameplayTags& InitTags = FJCGameplayTags::Get();
	if (CurrentState == FJCGameplayTags::Get().InitState_DataAvailable && DesiredState == FJCGameplayTags::Get().InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AJCPlayerState* JCPS = GetPlayerState<AJCPlayerState>();
		if (!ensure(Pawn && JCPS))
		{
			return;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UJCPawnData* PawnData = nullptr;

		if (UJCPawnExtensionComponent* PawnExtComp = UJCPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UJCPawnData>();
		
			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			// PawnExtComp->InitializeAbilitySystem(LyraPS->GetLyraAbilitySystemComponent(), LyraPS);
		}

		// if (ALyraPlayerController* LyraPC = GetController<ALyraPlayerController>())
		if (APlayerController* LyraPC = GetController<APlayerController>()) // Temp
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// if (bIsLocallyControlled && PawnData)
		// {
		// 	if (ULyraCameraComponent* CameraComponent = ULyraCameraComponent::FindCameraComponent(Pawn))
		// 	{
		// 		CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		// 	}
		// }
	}
}

void UJCPawnInputComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UJCPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FJCGameplayTags::Get().InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UJCPawnInputComponent::CheckDefaultInitialization()
{
	const FJCGameplayTags& InitTags = FJCGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UJCPawnInputComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UJCPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(FJCGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UJCPawnInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UJCPawnInputComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	// const ULyraLocalPlayer* LP = Cast<ULyraLocalPlayer>(PC->GetLocalPlayer());
	// check(LP);

	// UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(); // Temp
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UJCPawnExtensionComponent* PawnExtComp = UJCPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UJCPawnData* PawnData = PawnExtComp->GetPawnData<UJCPawnData>())
		{
			if (const UJCInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FJCGameplayTags& GameplayTags = FJCGameplayTags::Get();
	
				// // Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
				// for (const FMappableConfigPair& Pair : DefaultInputConfigs)
				// {
				// 	if (Pair.bShouldActivateAutomatically && Pair.CanBeActivated())
				// 	{
				// 		FModifyContextOptions Options = {};
				// 		Options.bIgnoreAllPressedKeysUntilRelease = false;
				// 		// Actually add the config to the local player							
				// 		Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);	
				// 	}
				// }

				if(DefaultInputMappingContext)
				{
					Subsystem->AddMappingContext(DefaultInputMappingContext.Get(), 0);
				}
				
				UJCEnhancedInputComponent* LyraIC = CastChecked<UJCEnhancedInputComponent>(PlayerInputComponent);
				LyraIC->AddInputMappings(InputConfig, Subsystem);

				// TArray<uint32> BindHandles;
				// LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

				LyraIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
				LyraIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
				// LyraIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
				// LyraIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
				// LyraIC->BindNativeAction(InputConfig, GameplayTags.InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
 
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

// void ULyraHeroComponent::AddAdditionalInputConfig(const ULyraInputConfig* InputConfig)
// {
// 	TArray<uint32> BindHandles;
//
// 	const APawn* Pawn = GetPawn<APawn>();
// 	if (!Pawn)
// 	{
// 		return;
// 	}
//
// 	ULyraInputComponent* LyraIC = Pawn->FindComponentByClass<ULyraInputComponent>();
// 	check(LyraIC);
//
// 	const APlayerController* PC = GetController<APlayerController>();
// 	check(PC);
//
// 	const ULocalPlayer* LP = PC->GetLocalPlayer();
// 	check(LP);
//
// 	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
// 	check(Subsystem);
//
// 	if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
// 	{
// 		LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
// 	}
// }
//
// void ULyraHeroComponent::RemoveAdditionalInputConfig(const ULyraInputConfig* InputConfig)
// {
// 	//@TODO: Implement me!
// }

bool UJCPawnInputComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UJCPawnInputComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// // If the player has attempted to move again then cancel auto running
	// if (ALyraPlayerController* LyraController = Cast<ALyraPlayerController>(Controller))
	// {
	// 	LyraController->SetIsAutoRunning(false);
	// }
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UJCPawnInputComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

