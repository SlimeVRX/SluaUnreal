// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameCoreGameModeBase.generated.h"

/**
 * GameCoreGameModeBase handles the core game logic including 
 * spawning the map and managing the game state.
 */
UCLASS()
class SLUAUNREAL_API AGameCoreGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Constructor
	AGameCoreGameModeBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Reference to the map generator BP class
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSubclassOf<AActor> MapGeneratorClass;
};
