// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/MapGenerator.h"
#include "Components/SceneComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMapGenerator::AMapGenerator()
{
 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;

	// Create and set the root component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootSceneComponent->SetMobility(EComponentMobility::Static); // Đặt Mobility là Static để phù hợp với các tile
	RootComponent = RootSceneComponent;
}

// Called when the game starts or when spawned
void AMapGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("MapGenerator::BeginPlay - Starting tile grid generation at location: %s"), *GetActorLocation().ToString());
	
	// Generate the tile grid once the game starts
	GenerateTileGrid();
	
	UE_LOG(LogTemp, Warning, TEXT("MapGenerator::BeginPlay - Completed tile grid generation, created %d tiles"), Tiles.Num());
}

// Called every frame
void AMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapGenerator::GenerateTileGrid()
{
	UE_LOG(LogTemp, Warning, TEXT("MapGenerator::GenerateTileGrid - Starting generation at %s"), *GetName());

	// Check if we have the necessary material
	if (!TileMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("MapGenerator: TileMaterial not set. Cannot generate tile grid."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MapGenerator: Using material %s"), *TileMaterial->GetName());
	}

	// Verify the root component is ready
	if (!RootComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("MapGenerator: RootComponent is null! Cannot attach tiles."));
		return;
	}

	// Clear existing tiles if any
	UE_LOG(LogTemp, Warning, TEXT("MapGenerator: Clearing %d existing tiles"), Tiles.Num());
	for (auto* Tile : Tiles)
	{
		if (Tile)
		{
			Tile->DestroyComponent();
		}
	}
	Tiles.Empty();

	// Kích thước thực của mỗi ô sau khi scale
	float ActualTileSize = TileSize;
	
	// Tính toán khoảng cách giữa các ô
	float TotalSpacing = ActualTileSize + TileSpacing;
	

	// Calculate the center offset to center the grid
	float CenterOffsetX = (GridSizeX - 1) * TotalSpacing * 0.5f;
	float CenterOffsetY = (GridSizeY - 1) * TotalSpacing * 0.5f;

	UE_LOG(LogTemp, Warning, TEXT("MapGenerator: Grid config - Size: %dx%d, TileSize: %.1f, Spacing: %.1f, Height: %.1f"), 
		GridSizeX, GridSizeY, TileSize, TileSpacing, TileHeight);
	UE_LOG(LogTemp, Warning, TEXT("MapGenerator: TotalSpacing: %.1f, CenterOffset: (%.1f, %.1f)"), 
		TotalSpacing, CenterOffsetX, CenterOffsetY);

	// Find a default cube mesh if TileMesh is not set
	if (!TileMesh)
	{
		// Try to find the engine cube mesh
		static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Engine/BasicShapes/Cube"));
		if (CubeMeshFinder.Succeeded())
		{
			TileMesh = CubeMeshFinder.Object;
			UE_LOG(LogTemp, Warning, TEXT("MapGenerator: Using default cube mesh"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MapGenerator: Failed to find default cube mesh and no TileMesh set"));
			return;
		}
	}

	// Create tiles in a grid pattern
	for (int32 Y = 0; Y < GridSizeY; Y++)
	{
		for (int32 X = 0; X < GridSizeX; X++)
		{
			// Generate a unique name for this tile
			FString TileName = FString::Printf(TEXT("Tile_%d_%d"), X, Y);
			
			// Create a new static mesh component for this tile
			UStaticMeshComponent* TileComponent = NewObject<UStaticMeshComponent>(this, *TileName);
			if (!TileComponent)
			{
				UE_LOG(LogTemp, Error, TEXT("MapGenerator: Failed to create StaticMeshComponent for tile %s"), *TileName);
				continue;
			}

			// Register the component to include it in the game
			TileComponent->RegisterComponent();
			UE_LOG(LogTemp, Log, TEXT("MapGenerator: Created tile %s"), *TileName);
			
			// Set the mesh and material
			TileComponent->SetStaticMesh(TileMesh);
			TileComponent->SetMaterial(0, TileMaterial);
			
					// Lấy kích thước thực của mesh (BEFORE scaling) để tính toán chính xác
			FVector MeshSize = FVector::ZeroVector;
			if (TileMesh)
			{
				FBoxSphereBounds Bounds = TileMesh->GetBounds();
				MeshSize = Bounds.BoxExtent * 2.0f; // BoxExtent là 1/2 kích thước
			}

			// Tính toán Scale để mesh sau khi scale có kích thước đúng TileSize
			FVector NewScale = FVector(1.0f, 1.0f, 1.0f); // scale mặc định

			if (!MeshSize.IsNearlyZero())
			{
				// Để kích thước thực của tile sau scaling = TileSize
				// Công thức: RealSize = OriginalSize * Scale
				// Nên: Scale = DesiredSize / OriginalSize
				NewScale.X = TileSize / MeshSize.X;
				NewScale.Y = TileSize / MeshSize.Y;
				NewScale.Z = TileHeight / MeshSize.Z;
				
				// Đảm bảo tỉ lệ XY giống nhau để không bị biến dạng
				float MinScale = FMath::Min(NewScale.X, NewScale.Y);
				NewScale.X = NewScale.Y = MinScale;
			}
			else
			{
				// Fallback nếu không tính được kích thước mesh
				NewScale = FVector(TileSize/100.0f, TileSize/100.0f, TileHeight/100.0f);
				UE_LOG(LogTemp, Warning, TEXT("MapGenerator: Using fallback scale for tile %s"), *TileName);
			}

			TileComponent->SetRelativeScale3D(NewScale);
			UE_LOG(LogTemp, Log, TEXT("MapGenerator: Set tile %s scale to %s for size %.1fx%.1fx%.1f"), 
				*TileName, *NewScale.ToString(), TileSize, TileSize, TileHeight);
			
			// Calculate position - center the grid with exact spacing
			float PosX = (X * TotalSpacing) - CenterOffsetX;
			float PosY = (Y * TotalSpacing) - CenterOffsetY;
			
			// Đặt vị trí, nâng ô lên 5 đơn vị trên trục Z để tránh nhấp nháy
			TileComponent->SetRelativeLocation(FVector(PosX, PosY, 5.0f));
			
			FVector WorldPos = TileComponent->GetComponentLocation();
			UE_LOG(LogTemp, Log, TEXT("MapGenerator: Tile %s position - Relative: (%.1f, %.1f, 0), World: %s"), 
				*TileName, PosX, PosY, *WorldPos.ToString());
			
			// Set up physics and collision
			TileComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			TileComponent->SetCollisionProfileName(TEXT("BlockAll"));
			// Đảm bảo Mobility là Static và khớp với RootComponent
			TileComponent->SetMobility(EComponentMobility::Static);
			
			// Kích hoạt khả năng phản hồi overlap events cho gameplay
			TileComponent->SetGenerateOverlapEvents(true);
			
			// Enable shadows
			TileComponent->SetCastShadow(true);
			
			// Attach to the root component (sử dụng KeepRelativeTransform vì cả hai đều là static)
			if (!TileComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform))
			{
				UE_LOG(LogTemp, Error, TEXT("MapGenerator: Failed to attach tile %s to root component"), *TileName);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("MapGenerator: Successfully attached tile %s to root"), *TileName);
			}
			
			// Đảm bảo component được hiển thị
			TileComponent->SetVisibility(true);
			
			// Add to our array for reference
			Tiles.Add(TileComponent);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("MapGenerator: Generated %d tiles in a %dx%d grid with size %.1f and spacing %.1f."), 
		Tiles.Num(), GridSizeX, GridSizeY, TileSize, TileSpacing);
	
	// debug code removed as requested
}
