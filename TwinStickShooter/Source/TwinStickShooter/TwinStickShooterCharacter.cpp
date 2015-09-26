// Fill out your copyright notice in the Description page of Project Settings.

#include "TwinStickShooter.h"
#include "TwinStickShooterCharacter.h"
#include "TwinStickShooterCharacter.h"
#include "TwinStickShooterProjectile.h"
#include "TimerManager.h"

const FName ATwinStickShooterCharacter::MoveForwardBinding("MoveForward");
const FName ATwinStickShooterCharacter::MoveRightBinding("MoveRight");
const FName ATwinStickShooterCharacter::FireForwardBinding("FireForward");
const FName ATwinStickShooterCharacter::FireRightBinding("FireRight");
const FName ATwinStickShooterCharacter::ZoomTriggerBinding("ZoomTrigger");

ATwinStickShooterCharacter::ATwinStickShooterCharacter()
{
		
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TwinStickShooterCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TwinStickShooterCamera"));
	TwinStickShooterCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TwinStickShooterCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Movement
	MoveSpeed = 1000.0f;
	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true; 
	CanShoot = true;
}

void ATwinStickShooterCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// set up gameplay key bindings
	InputComponent->BindAxis(MoveForwardBinding);
	InputComponent->BindAxis(MoveRightBinding);
	InputComponent->BindAxis(FireForwardBinding);
	InputComponent->BindAxis(FireRightBinding);
	InputComponent->BindAxis(ZoomTriggerBinding);
}

void ATwinStickShooterCharacter::Tick(float DeltaSeconds)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed;
	
	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		AddMovementInput(Movement, 1.0f, false);
	}


	// Create fire direction vector
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);
	FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);
	// If we are pressing fire stick in a direction
	if (FireDirection.SizeSquared() <= 0.000001f)
		FireDirection = MoveDirection;
	if (FireDirection.SizeSquared() <= 0.000001f)
		FireDirection = GetActorForwardVector();

	const float ZoomTriggerValue = GetInputAxisValue(ZoomTriggerBinding);
	// Try and fire a shot
	if (ZoomTriggerValue > 0.7f)
	{
		FireShot_Implementation(FireDirection);
	}
	
	OnTick(DeltaSeconds);
}

void ATwinStickShooterCharacter::FireShot_Implementation(FVector FireDirection)
{
	if (!CanShoot)
		return;

	// If we it's ok to fire again
	if (bCanFire == true)
	{
		FireShot(FireDirection);

		const FRotator FireRotation = FireDirection.Rotation();
		// Spawn projectile at an offset from this pawn
		const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			//Spawn only server side at it will automatically spawns a replicate on client's side
			if (Role >= ROLE_Authority)
			{
				// spawn the projectile
				if (ProjectileClass != NULL)
				{
					FActorSpawnParameters params;
					params.Instigator = this;
					World->SpawnActor<AActor>(ProjectileClass, SpawnLocation, FireRotation, params);
				}
				//ATwinStickShooterProjectile* projectile = World->SpawnActor<ATwinStickShooterProjectile>(SpawnLocation, FireRotation);
			}

			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ATwinStickShooterCharacter::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}
		}

		bCanFire = false; 
	}

	// If this next check succeeds, we are *not* the authority, meaning we are a network client.
	// In this case we also want to call the server function to tell it to change the bSomeBool property as well.
	if (Role < ROLE_Authority)
	{
		ServerFireShot(FireDirection);
	}
}

bool ATwinStickShooterCharacter::ServerFireShot_Validate(FVector FireDirection)
{
	return true;
}

void ATwinStickShooterCharacter::ServerFireShot_Implementation(FVector FireDirection)
{
	// This function is only called on the server (where Role == ROLE_Authority), called over the network by clients.
	// We need to call FireShot()!
	// Inside that function, Role == ROLE_Authority, so it won't try to call ServerSetSomeBool() again.
	FireShot_Implementation(FireDirection);
}

void ATwinStickShooterCharacter::ShotTimerExpired()
{
	bCanFire = true;
}

