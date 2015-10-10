// Fill out your copyright notice in the Description page of Project Settings.

#include "TwinStickShooter.h"
#include "TwinStickShooterCharacter.h"
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
	OnTick(DeltaSeconds);
}