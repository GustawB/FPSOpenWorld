// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSOpenWorldCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameModeBase.h"
#include "FPSOpenWorldGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FPSOPENWORLD_API AFPSOpenWorldGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSOpenWorldGameModeBase();

	virtual void BeginPlay() override;

};
