// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFeaturesToolkitBPLibrary.h"
#include "GameFeaturesToolkit.h"

#include "GameFeaturesSubsystem.h"

//DEFINE_LOG_CATEGORY(LogGameFeaturesToolkit)

UGameFeaturesToolkitBPLibrary::UGameFeaturesToolkitBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UGameFeaturesToolkitBPLibrary::SetGameFeaturePluginActiveByName(const FString& Name, const bool bNewActive)
{
	if(Name.IsEmpty())
	{
		//UE_LOG(LogGameFeaturesToolkit, Warning, TEXT("Game feature plugin name is empty."));
		return;
	}

	FString PluginURL;
	UGameFeaturesSubsystem& GFS = UGameFeaturesSubsystem::Get();

	if(!GFS.GetPluginURLByName(Name, PluginURL))
	{
		return;
	}

	// Is Installed & Registered?
	if(!GFS.IsGameFeaturePluginInstalled(PluginURL) && !GFS.IsGameFeaturePluginRegistered(PluginURL))
	{
		return;	
	}
	// Is Loaded?
	if(!GFS.IsGameFeaturePluginLoaded(PluginURL))
	{
		return;
	}

	if(bNewActive)
	{
		GFS.LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete());
	}
	else
	{
		GFS.DeactivateGameFeaturePlugin(PluginURL);
	}
	
}

void UGameFeaturesToolkitBPLibrary::LoadGameFeaturePluginByName(const FString& Name)
{
	FString PluginURL;
	UGameFeaturesSubsystem& GFS = UGameFeaturesSubsystem::Get();

	if (!GFS.GetPluginURLByName(Name, PluginURL))
	{
		return;
	}

	if(!GFS.IsGameFeaturePluginInstalled(PluginURL))
	{
		return;
	}

	UGameFeaturesSubsystem::Get().LoadGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete());
}

	

void UGameFeaturesToolkitBPLibrary::UnloadedGameFeaturePluginByName(const FString& Name)
{
	FString PluginURL;
	UGameFeaturesSubsystem& GFS = UGameFeaturesSubsystem::Get();

	if (!GFS.GetPluginURLByName(Name, PluginURL))
	{
		return;
	}

	if(!GFS.IsGameFeaturePluginLoaded(PluginURL))
	{
		return;
	}

	GFS.UnloadGameFeaturePlugin(PluginURL, true);
}

//void UGameFeaturesToolkitBPLibrary::SetGameFeaturePluginRegisteredByName(const FString& Name, const bool bRegistered)
//{
//}
