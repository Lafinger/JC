// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// #include "CommonGameInstance.h"
#include "UObject/UObjectGlobals.h"

#include "JCGameInstance.generated.h"

class UObject;

UCLASS(Config = Game)
class JC_API UJCGameInstance : public UGameInstance
/*: public UCommonGameInstance*/
{
	GENERATED_BODY()

public:

	UJCGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	APlayerController* GetPrimaryPlayerController() const;
	
	// virtual bool CanJoinRequestedSession() const override;

protected:

	virtual void Init() override;
	virtual void Shutdown() override;
};
