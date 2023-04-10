// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "JCAssetManager.generated.h"

//class UJCGameData;

struct FJCBundles
{
	static const FName Equipped;
};

/**
 * 
 */
UCLASS(Config = Game)
class JC_API UJCAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	// Returns the AssetManager singleton object.
	static UJCAssetManager& Get();
	// const UJCGameData& GetGameData();

protected:
	//~UAssetManager interface
	virtual void StartInitialLoading() override;
//#if WITH_EDITOR
//	virtual void PreBeginPIE(bool bStartSimulate) override;
//#endif
//	//~End of UAssetManager interface
};
