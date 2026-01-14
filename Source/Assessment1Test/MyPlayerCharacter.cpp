#include "MyPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsRightClickHeld = false; // Start off

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AMyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
    // Check if selected Widget in the Blueprint
    if (HUDWidgetClass)
    {
        CurrentHUD = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
        if (CurrentHUD)
        {
            CurrentHUD->AddToViewport(); // Put crosshair on screen
        }
    }
}

void AMyPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Link the Right Click button
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &AMyPlayerCharacter::StartRightClick);
	PlayerInputComponent->BindAction("RightClick", IE_Released, this, &AMyPlayerCharacter::StopRightClick);

	// Link the mouse movement custom functions
	PlayerInputComponent->BindAxis("Turn", this, &AMyPlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyPlayerCharacter::LookUp);

	// Jump logic
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyPlayerCharacter::ExecuteParkourMove);

	// Interact 
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyPlayerCharacter::OnInteract);
}

// Turn the switch ON
void AMyPlayerCharacter::StartRightClick() { bIsRightClickHeld = true; }

// Turn the switch OFF
void AMyPlayerCharacter::StopRightClick() { bIsRightClickHeld = false; }

// Only rotate the camera if the switch is on
void AMyPlayerCharacter::Turn(float Value)
{
	if (bIsRightClickHeld) { AddControllerYawInput(Value); }
}

void AMyPlayerCharacter::LookUp(float Value)
{
	if (bIsRightClickHeld) { AddControllerPitchInput(Value); }
}

void AMyPlayerCharacter::ExecuteParkourMove()
{
	if (ParkourMontage)
	{
		PlayAnimMontage(ParkourMontage);
		FVector LaunchVelocity = GetActorForwardVector() * 600.0f + FVector(0, 0, 400.0f);
		LaunchCharacter(LaunchVelocity, true, true);
	}
}

void AMyPlayerCharacter::OnInteract()
{
    if (HeldItem)
    {
        DropItem(); // If holding something, drop it
    }
    else
    {
        PickUpItem(); // If not, try pick up
    }
}

void AMyPlayerCharacter::PickUpItem()
{
    // Find Camera  
    UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();

    if (Camera == nullptr)
    {
        return; // If no camera  -> stop 
    }

    // Get Location + Direction from camera 
    FVector Start = Camera->GetComponentLocation();
    FVector ForwardVector = Camera->GetForwardVector();
    FVector End = Start + (ForwardVector * 500.0f); // 500 unit reach

    FHitResult Hit;
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);

    // Shoot the Laser
    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

    // Check for the "PuzzleBlock" tag
    if (bHit && Hit.GetActor()->ActorHasTag("PuzzleBlock"))
    {
        HeldItem = Hit.GetActor();

        // Disable physics to carry it
        UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(HeldItem->GetRootComponent());
        if (RootComp)
        {
            RootComp->SetSimulatePhysics(false);

            // Set everything to block first, then override the specific channels to ignore
            RootComp->SetCollisionResponseToAllChannels(ECR_Block);
            RootComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
            RootComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
            RootComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore); // Stops hitting walls while held
        }

        // Attach to the "HoldSocket" 
        HeldItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "HoldSocket");
    }
}

void AMyPlayerCharacter::DropItem()
{
    if (HeldItem)
    {
        // Detach from the hand
        HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(HeldItem->GetRootComponent());
        if (RootComp)
        {
            // Turn physics back on so it falls
            RootComp->SetSimulatePhysics(true);

            RootComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

            // Re-enable collisions for all channels
            RootComp->SetCollisionResponseToAllChannels(ECR_Block);
            RootComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
            RootComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);

            // Overlap events -> on so the plate sees cube
            RootComp->SetGenerateOverlapEvents(true);

        
            RootComp->WakeRigidBody();
        }

        HeldItem = nullptr; // Forget item
    }
}