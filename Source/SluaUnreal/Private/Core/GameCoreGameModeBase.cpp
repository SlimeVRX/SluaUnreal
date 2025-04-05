// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GameCoreGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AGameCoreGameModeBase::AGameCoreGameModeBase()
{
    // Set default values
    PrimaryActorTick.bCanEverTick = false;
}

void AGameCoreGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // Spawn the map generator if a class is specified
    if (MapGeneratorClass)
    {
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(MapGeneratorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        UE_LOG(LogTemp, Log, TEXT("GameCoreGameModeBase: Map generator spawned."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameCoreGameModeBase: No MapGeneratorClass specified. Map will not be generated."));
    }
}
