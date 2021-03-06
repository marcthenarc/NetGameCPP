

#include "NetGameCPP.h"
#include "Bomb.h"
#include "UnrealNetwork.h"
#include "NetGameCPPCharacter.h"


#define FromFloat(x) FString::Printf(TEXT("%0.2f"), x)

ABomb::ABomb(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Armed(false)
	, MaterialColorParamName(TEXT("Color"))
	, BombMIB(NULL)
	, FuseTime(3.0f)
	, ExplosionRadius(300.0f)
	, ExplosionDamage(25.0f)
{

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(55.0f);

	CollisionComp->SetCollisionProfileName(FName(TEXT("BlockAll")));

	RootComponent = CollisionComp;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 1000.f;
    ProjectileMovement->MaxSpeed = 3000;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness  = 0.6f;
	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &ABomb::OnProjectileBounce);
}

void ABomb::OnConstruction(const FTransform &Transform)
{
	UPrimitiveComponent *pc = Cast<UPrimitiveComponent>(GetComponent(RootComponent, "BombMesh"));

	if (pc)
	{
		BombMIB = pc->CreateDynamicMaterialInstance(0, NULL);
		SetBombColor(FLinearColor::Black);
	}
}

void ABomb::InitVelocity(const FVector& ShootDirection)
{
	if (ProjectileMovement)
	{
		// set the projectile's velocity to the desired direction
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}

USceneComponent * ABomb::GetComponent(USceneComponent *parent, const FString& childLabel)
{
	if (!parent)
		return NULL;

	USceneComponent *child = NULL;

	for (int i=0; i<parent->GetNumChildrenComponents(); i++)
	{
		child = parent->GetChildComponent(i);

		if (child && child->GetName() == childLabel)
			break;
	}

	return child;
}

void ABomb::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (Role == ROLE_Authority)
	{
		Armed = true;

		// You must call the rep function yourself for the server or it will never change color.
		OnRep_Armed();

		GetWorldTimerManager().SetTimer(BombTimerHandle, this, &ABomb::OnFuseExpired,FuseTime, false);
	}
}

void ABomb::OnFuseExpired()
{
	if (Role == ROLE_Authority)
	{
		ApplyExplosionDamage();
	}
}

void ABomb::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
    // Replicate to everyone
	DOREPLIFETIME(ABomb, Armed);
}

void ABomb::OnRep_Armed()
{
	if (Armed)
		SetBombColor(FLinearColor::Red);
}

void ABomb::SetBombColor(const FLinearColor& bombColor)
{
	if (BombMIB)
		BombMIB->SetVectorParameterValue(MaterialColorParamName, bombColor);
}

void ABomb::ApplyExplosionDamage()
{
	for ( TActorIterator<ANetGameCPPCharacter> aItr(GetWorld()); aItr; ++aItr )
	{
		if ( GetDistanceTo(*aItr) <= ExplosionRadius)
			UGameplayStatics::ApplyDamage(*aItr, ExplosionDamage, GetInstigatorController(), this, UDamageType::StaticClass());
	}

	OnExplosion();
	SetLifeSpan(2.0f);
}

void ABomb::OnExplosion_Implementation()
{
	if (ExplosionEffects)
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffects, GetActorLocation(), FRotator::ZeroRotator, true);

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}
