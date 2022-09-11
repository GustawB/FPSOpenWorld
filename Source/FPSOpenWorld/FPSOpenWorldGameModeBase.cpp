// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSOpenWorldGameModeBase.h"

AFPSOpenWorldGameModeBase::AFPSOpenWorldGameModeBase() : Super()
{
	//set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn>PlayerPawnClassFInder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFInder.Class;
}

void AFPSOpenWorldGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}
