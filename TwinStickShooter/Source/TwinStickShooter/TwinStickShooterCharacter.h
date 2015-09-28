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

	// Begin Actor Interface
	UFUNCTION(BlueprintImplementableEvent, Category = "Tick")
	void OnTick(float DeltaSeconds);
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;
	static const FName ZoomTriggerBinding;

private:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

};
