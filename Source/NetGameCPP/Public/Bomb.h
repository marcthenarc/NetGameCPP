

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

	void InitVelocity(const FVector& ShootDirection);

	virtual void OnConstruction(const FTransform &Transform) override;

protected:

	UPROPERTY()
	UMaterialInstanceDynamic* BombMIB;

	UPROPERTY(ReplicatedUsing=OnRep_Armed)
	bool Armed;

	UFUNCTION()
	void OnRep_Armed();

	UFUNCTION()
	void OnProjectileBounce( const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	FName MaterialColorParamName;
	FColor MaterialColor;
	USceneComponent * GetComponent(USceneComponent *parent, const FString& childLabel);

	void SetBombColor(const FLinearColor& bombColor);
};
