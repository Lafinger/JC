// Fill out your copyright notice in the Description page of Project Settings.


#include "JCAssetManager.h"

#include "JCGameplayTags.h"
//#include "Logging/LogMacros.h"

const FName FJCBundles::Equipped("Equipped");

//DEFINE_LOG_CATEGORY(LogJC)

UJCAssetManager& UJCAssetManager::Get()
{
	check(GEngine);

	if (UJCAssetManager* Singleton = Cast<UJCAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	//UE_LOG(LogJC, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to JCAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UJCAssetManager>();
}

// const UJCGameData& UJCAssetManager::GetGameData()
// {
// 	return GetOrLoadTypedGameData<ULyraGameData>(LyraGameDataPath);
// }

void UJCAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FJCGameplayTags::InitializeNativeTags();
}

//void UJCAssetManager::PreBeginPIE(bool bStartSimulate)
//{
//	Super::PreBeginPIE(bStartSimulate);
//}
