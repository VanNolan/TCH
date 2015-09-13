// Fill out your copyright notice in the Description page of Project Settings.

#include "TwinStickShooter.h"
#include "HealthComponent.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	fRegenPerSec = 0.0f;
	iLifeMax = 100;
	iLifeCur = iLifeMax;
}

void UHealthComponent::ApplyDamages(int32 Damages)
{
	if (Damages > iLifeCur)
	{
		iLifeCur = 0;
		OnZeroHealthPointReached.Broadcast(); 
	}
	else
	{
		iLifeCur -= Damages;
	}
}
// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	iLifeCur = iLifeMax;
	
}


// Called every frame
void UHealthComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

