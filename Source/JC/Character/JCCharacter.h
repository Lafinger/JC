// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "InputActionValue.h"
#include "JCCharacter.generated.h"

class UInputComponent;
class UJCInputConfig;
class UJCPawnExtensionComponent;
class UJCPawnInputComponent;
/**
 * 
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class JC_API AJCCharacter : public AModularCharacter
{
	GENERATED_BODY()
public:
	AJCCharacter();
	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JC|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UJCPawnExtensionComponent> PawnExtensionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JC|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UJCPawnInputComponent> PawnInputComponent;
};
