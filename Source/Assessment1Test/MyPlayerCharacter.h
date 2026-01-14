#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyPlayerCharacter.generated.h"

UCLASS()
class ASSESSMENT1TEST_API AMyPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> HUDWidgetClass;
	// If the button is held down
	bool bIsRightClickHeld;

	// Func switch on and off
	void StartRightClick();
	void StopRightClick();

	// Look around funcs
	void LookUp(float Value);
	void Turn(float Value);

	// The Input function for 'E'
	void OnInteract();

	// Variable to remember the block we are holding
	class AActor* HeldItem;

	// Helper funcs
	void PickUpItem();
	void DropItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour")
	class UAnimMontage* ParkourMontage;
	class UUserWidget* CurrentHUD;

	void ExecuteParkourMove();

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};