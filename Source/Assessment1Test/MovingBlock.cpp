#include "MovingBlock.h"

AMovingBlock::AMovingBlock()
{
	PrimaryActorTick.bCanEverTick = true;
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	RootComponent = BlockMesh;
}

void AMovingBlock::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
}

void AMovingBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the mesh is missing, stop immediately
	if (!BlockMesh) return;

	FVector CurrentLocation = StartLocation;
	float Time = GetWorld()->GetTimeSeconds();
	float Movement = FMath::Sin(Time * Speed) * Amplitude;

	// Use the Dropdown choice
	if (MovementType == EMoveDir::Vertical)
	{
		CurrentLocation.Z += Movement;
	}
	else if (MovementType == EMoveDir::Horizontal)
	{
		CurrentLocation.Y += Movement;
	}

	SetActorLocation(CurrentLocation);
}