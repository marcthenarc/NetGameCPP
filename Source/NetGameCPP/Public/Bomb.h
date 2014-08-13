

#pragma once

#include "GameFramework/Actor.h"
#include "Bomb.generated.h"

/**
 * 
 */
UCLASS()
class NETGAMECPP_API ABomb : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category=Bomb)
	TSubobjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement)
	TSubobjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY()
	UMaterialInstanceDynamic* BombMIB;

	void InitVelocity(const FVector& ShootDirection);

	virtual void OnConstruction(const FTransform &Transform) override;
};
