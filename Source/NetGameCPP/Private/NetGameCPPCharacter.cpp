// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "NetGameCPP.h"
#include "NetGameCPPCharacter.h"
#include "UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
// ANetGameCPPCharacter

ANetGameCPPCharacter::ANetGameCPPCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
	, Health(0.0f)
	, BombCount(0)
	, MaxHealth(100.0f)
	, MaxBombCount(3)
{
	// Set size for collision capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	CharacterMovement->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	CharacterMovement->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	CharacterMovement->JumpZVelocity = 600.f;
	CharacterMovement->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUseControllerViewRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUseControllerViewRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetGameCPPCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("SpawnBomb", IE_Pressed, this, &ANetGameCPPCharacter::SpawnBomb);

	InputComponent->BindAxis("MoveForward", this, &ANetGameCPPCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ANetGameCPPCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ANetGameCPPCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ANetGameCPPCharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ANetGameCPPCharacter::TouchStarted);
}


void ANetGameCPPCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ANetGameCPPCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANetGameCPPCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ANetGameCPPCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ANetGameCPPCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ANetGameCPPCharacter::SpawnBomb()
{
	if (HasBombs())
		Server_AttemptSpawnBomb();
}

void ANetGameCPPCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
    // Replicate to everyone
	DOREPLIFETIME(ANetGameCPPCharacter, Health);
	DOREPLIFETIME(ANetGameCPPCharacter, BombCount);
}

void ANetGameCPPCharacter::BeginPlay()
{
	if (Role == ROLE_Authority)
		InitAttributes();
}

void ANetGameCPPCharacter::InitAttributes()
{
	if (Role == ROLE_Authority)
	{
		InitHealth();
		InitBombs();
	}
}

void ANetGameCPPCharacter::InitHealth()
{
	Health = MaxHealth;
}

void ANetGameCPPCharacter::InitBombs()
{
	BombCount = MaxBombCount;
}

bool ANetGameCPPCharacter::HasBombs()
{
	return (BombCount > 0);
}

bool ANetGameCPPCharacter::Server_AttemptSpawnBomb_Validate()
{
	return true;
}

void ANetGameCPPCharacter::Server_AttemptSpawnBomb_Implementation()
{
	if (Role == ROLE_Authority)
	{
		if (HasBombs())
			AttemptSpawnBomb();
	}
}

void ANetGameCPPCharacter::AttemptSpawnBomb()
{
	if (Role == ROLE_Authority && HasBombs())
	{
        // try and fire a projectile
        if (BombClass != NULL)
        {
            // Get the camera transform
            FVector ExitLoc  = GetActorLocation() + GetActorForwardVector() * 100;
            FRotator ExitRot = GetActorRotation();
	
            UWorld* const World = GetWorld();

            if (World)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.Owner = this;
                SpawnParams.Instigator = Instigator;

                // spawn the projectile at the muzzle
                ABomb* const Bomb = World->SpawnActor<ABomb>(BombClass, ExitLoc, ExitRot, SpawnParams);
                if (Bomb)
                {
                    // find launch direction
                    Bomb->InitVelocity(ExitRot.Vector());
					BombCount -= 1;
                }
            }
        }
    }
}

float ANetGameCPPCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float realDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (realDamage >= 0.0f)
		Health -= realDamage;

	if (Health <= 0.0f)
		InitHealth();

	return realDamage;
}
