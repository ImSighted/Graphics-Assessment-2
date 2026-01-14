#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingBlock.generated.h"

// Dropdown options
UENUM(BlueprintType)
enum class EMoveDir : uint8
{
	Vertical    UMETA(DisplayName = "Up and Down"),
	Horizontal  UMETA(DisplayName = "Side to Side"),
	Stationary  UMETA(DisplayName = "Stay Still")
};

UCLASS()
class ASSESSMENT1TEST_API AMovingBlock : public AActor
{
	GENERATED_BODY()

public:
	AMovingBlock();
	virtual void Tick(float DeltaTime) override;

	// The Mesh
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BlockMesh;

	// The Dropdown Menu
	UPROPERTY(EditAnywhere, Category = "Movement")
	EMoveDir MovementType = EMoveDir::Vertical;

	// Adjustable Distance and Speed
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Amplitude = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 2.0f;

protected:
	virtual void BeginPlay() override;
	FVector StartLocation;
};