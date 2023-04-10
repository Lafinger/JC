// Fill out your copyright notice in the Description page of Project Settings.


#include "JCCharacter.h"

#include "JCPawnExtensionComponent.h"
#include "JCPawnInputComponent.h"


AJCCharacter::AJCCharacter()
{
	PawnExtensionComponent = CreateDefaultSubobject<UJCPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	// PawnInputComponent = CreateDefaultSubobject<UJCPawnInputComponent>(TEXT("JCPawnInputComponent"));
}

void AJCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtensionComponent->SetupPlayerInputComponent();
}
