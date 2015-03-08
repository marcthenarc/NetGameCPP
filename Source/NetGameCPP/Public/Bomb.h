

#pragma once

#include "GameFramework/Actor.h"
#include "ParticleDefinitions.h"
#include "Bomb.generated.h"

/**
 * 
 */
UCLASS()
class NETGAMECPP_API ABomb : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category=Bomb)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement)
	UProjectileMovementComponent* ProjectileMovement;

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

	UFUNCTION()
	void OnFuseExpired();

	FName MaterialColorParamName;
	FColor MaterialColor;
	float FuseTime;
	float ExplosionRadius;
	float ExplosionDamage;

	void SetBombColor(const FLinearColor& bombColor);
	USceneComponent * GetComponent(USceneComponent *parent, const FString& childLabel);
	void ApplyExplosionDamage();

	UFUNCTION(NetMulticast, Unreliable)
	void OnExplosion();

	/** Bomb class to spawn */
	 /** effect played on respawn */
	UPROPERTY(EditDefaultsOnly, Category=Explosion)
	UParticleSystem* ExplosionEffects;
};
