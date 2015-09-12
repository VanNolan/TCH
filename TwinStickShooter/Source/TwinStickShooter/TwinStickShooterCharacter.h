// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "TwinStickShooterCharacter.generated.h"

UCLASS()
class TWINSTICKSHOOTER_API ATwinStickShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TwinStickShooterCameraComponent;
	
	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
public:
	ATwinStickShooterCharacter();
	
	/** Returns TCHeroesCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTwinStickShooterCameraComponent() const { return TwinStickShooterCameraComponent; }

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		FVector GunOffset;

	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float FireRate;

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float MoveSpeed;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	/* Fire a shot in the specified direction */
	UFUNCTION(BlueprintNativeEvent, Category = "Firing")
	void FireShot(FVector FireDirection);
	UFUNCTION(Server, WithValidation, Reliable)
	void ServerFireShot(FVector FireDirection);
	void ServerFireShot_Implementation(FVector FireDirection);
	bool ServerFireShot_Validate(FVector FireDirection);

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;
	static const FName ZoomTriggerBinding;

	/** Projectile actor to create each time the character shoots*/
	UPROPERTY(Category = Bullet, EditAnywhere, BlueprintReadWrite)
	UClass* ProjectileClass;

private:

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
