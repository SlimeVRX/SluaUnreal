// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "MapGenerator.generated.h"

/**
 * MapGenerator is responsible for creating a grid of tiles that form the game map.
 * It generates a 12x12 grid of square tiles by default, which can be configured.
 */
UCLASS()
class SLUAUNREAL_API AMapGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Root component to attach all tiles to
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map")
	USceneComponent* RootSceneComponent;

	// Static mesh to use for each tile - if not set, a default cube will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	UStaticMesh* TileMesh;

	// The material to apply to each tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	UMaterialInterface* TileMaterial;

	// Size of each tile (width, length, height)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (ClampMin = "10.0"))
	float TileSize = 400.0f;

	// Spacing between tiles (0 = tiles touch each other, >0 = gap between tiles)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (ClampMin = "0.0"))
	float TileSpacing = 0.0f;

	// Height of each tile (when using default mesh)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (ClampMin = "1.0"))
	float TileHeight = 10.0f;

	// Bật debug visualization nâng cao
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	bool bEnhancedDebug = true;

	// Number of tiles in the X direction (width of the grid)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (ClampMin = "1", ClampMax = "50"))
	int32 GridSizeX = 12;

	// Number of tiles in the Y direction (height of the grid)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (ClampMin = "1", ClampMax = "50"))
	int32 GridSizeY = 12;

	// Store references to all created tiles
	UPROPERTY()
	TArray<UStaticMeshComponent*> Tiles;

	// Creates the tile grid
	void GenerateTileGrid();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
