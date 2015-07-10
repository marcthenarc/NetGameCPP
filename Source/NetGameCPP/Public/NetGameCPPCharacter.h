// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "Bomb.h"
#include "NetGameCPPCharacter.generated.h"

UCLASS(config=Game)
class ANetGameCPPCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	UCameraComponent* FollowCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(Replicated)
	float Health;

	UPROPERTY(Replicated)
	int32 BombCount;

	UPROPERTY(Replicated)
	int32 TSId;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Called when we spawn a bomb. */
	void SpawnBomb();

	void IncrementID();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent);
	// End of APawn interface

	float MaxHealth;
	int32 MaxBombCount;

	void InitAttributes();
	void InitHealth();
	void InitBombs();

	virtual void BeginPlay() override;

	bool HasBombs();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AttemptSpawnBomb();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetTSId(int32 id);

	/** Bomb class to spawn */
    UPROPERTY(EditDefaultsOnly, Category=Bomb)
    TSubclassOf<class ABomb> BombClass;

	/** Offset from the character's location */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
    FVector ExitOffset;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void AttemptSpawnBomb();
	void SetTSId(int32 id);
};

