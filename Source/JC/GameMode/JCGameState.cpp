// Fill out your copyright notice in the Description page of Project Settings.


#include "JCGameState.h"
#include "JCExperienceManagerComponent.h"


AJCGameState::AJCGameState(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ExperienceManagerComponent = CreateDefaultSubobject<UJCExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
