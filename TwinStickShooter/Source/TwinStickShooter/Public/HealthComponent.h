// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "GameFramework/Controller.h"
#include "HealthComponent.generated.h"

//Multi cast delegate can bind multiple functions, use AddDynamic(...) to add function to multicast bind
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FZeroHPReachedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TWINSTICKSHOOTER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	/** How much the health point refills per second*/
	UPROPERTY(Category = Health, EditAnywhere, BlueprintReadWrite)
		float fRegenPerSec;
	/** Maximum health point pool*/
	UPROPERTY(Category = Health, EditAnywhere, BlueprintReadWrite)
	int32 iLifeMax;
	/** Current health points value*/
	UPROPERTY(Category = Health, EditAnywhere, BlueprintReadWrite)
		int32 iLifeCur;

	/* Event called when Health points pool reaches 0 */
	UPROPERTY(BlueprintAssignable, Category = "Health")
		FZeroHPReachedDelegate OnZeroHealthPointReached;

	/* Function to call to apply damages */
	UFUNCTION(BlueprintCallable, Category = "Health")
		void ApplyDamages(int32 Damages, AController* Instigator);

	/* Function to call to reset health values*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		void Reset();

	UPROPERTY(Category = Health, BlueprintReadOnly)
		AController* LastInstigator;
};
