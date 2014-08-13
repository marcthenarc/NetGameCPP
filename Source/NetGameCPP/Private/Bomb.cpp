

#include "NetGameCPP.h"
#include "Bomb.h"


ABomb::ABomb(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

	CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(55.0f);
	CollisionComp->SetCollisionProfileName(FName(TEXT("BlockAll")));

	RootComponent = CollisionComp;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 1000.f;
    ProjectileMovement->MaxSpeed = 3000;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness  = 0.6f;
}

void ABomb::OnConstruction(const FTransform &Transform)
{
//	BombMIB = BombComp->CreateDynamicMaterialInstance(0, NULL);
}

void ABomb::InitVelocity(const FVector& ShootDirection)
{
	if (ProjectileMovement)
	{
		// set the projectile's velocity to the desired direction
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}


