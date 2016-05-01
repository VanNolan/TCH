// Fill out your copyright notice in the Description page of Project Settings.

#include "TwinStickShooter.h"
#include "FogOfWarManager.h"
#include "EngineUtils.h"

// Sets default values
AFogOfWarManager::AFogOfWarManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	textureRegions = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureSize, TextureSize);

	//15 Gaussian samples. Sigma is 2.0.
	//CONSIDER: Calculate the kernel instead, more flexibility...
	blurKernel.Init(0.0f, blurKernelSize);
	blurKernel[0] = 0.000489f;
	blurKernel[1] = 0.002403f;
	blurKernel[2] = 0.009246f;
	blurKernel[3] = 0.02784f;
	blurKernel[4] = 0.065602f;
	blurKernel[5] = 0.120999f;
	blurKernel[6] = 0.174697f;
	blurKernel[7] = 0.197448f;
	blurKernel[8] = 0.174697f;
	blurKernel[9] = 0.120999f;
	blurKernel[10] = 0.065602f;
	blurKernel[11] = 0.02784f;
	blurKernel[12] = 0.009246f;
	blurKernel[13] = 0.002403f;
	blurKernel[14] = 0.000489f;
}

AFogOfWarManager::~AFogOfWarManager() 
{
	if (FowThread) 
	{
		FowThread->ShutDown();
	}
}

// Called when the game starts or when spawned
void AFogOfWarManager::BeginPlay()
{
	Super::BeginPlay();	

	bIsDoneBlending = true;
	AFogOfWarManager::StartFOWTextureUpdate();
}

// Called every frame
void AFogOfWarManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (FOWTexture && LastFOWTexture && bHasFOWTextureUpdate && bIsDoneBlending) 
	{
		LastFOWTexture->UpdateResourceW();
		UpdateTextureRegions(LastFOWTexture, (int32)0, (uint32)1, textureRegions, (uint32)(4 * TextureSize), (uint32)4, (uint8*)LastFrameTextureData.GetData(), false);
		FOWTexture->UpdateResourceW();
		UpdateTextureRegions(FOWTexture, (int32)0, (uint32)1, textureRegions, (uint32)(4 * TextureSize), (uint32)4, (uint8*)TextureData.GetData(), false);
		bHasFOWTextureUpdate = false;
		bIsDoneBlending = false;
		//Trigger the blueprint update
		OnFowTextureUpdated(FOWTexture, LastFOWTexture);
	}
}

void AFogOfWarManager::StartFOWTextureUpdate() 
{
	if (!FOWTexture) 
	{
		FOWTexture = UTexture2D::CreateTransient(TextureSize, TextureSize);
		LastFOWTexture = UTexture2D::CreateTransient(TextureSize, TextureSize);
		int arraySize = TextureSize * TextureSize;
		TextureData.Init(FColor(100, 100, 100, 255), arraySize);
		LastFrameTextureData.Init(FColor(100, 100, 100, 255), arraySize);
		HorizontalBlurData.Init(0, arraySize);
		UnfoggedData.Init(false, arraySize);
		FowThread = new AFogOfWarWorker(this);
	}
}

void AFogOfWarManager::OnFowTextureUpdated_Implementation(UTexture2D* currentTexture, UTexture2D* lastTexture) 
{
	//Handle in blueprint
}

void AFogOfWarManager::RegisterFowActor(AActor* Actor) 
{
	if (Actor)
	{
		FowActors.AddUnique(Actor);
	}
}

void AFogOfWarManager::UnRegisterFowActor(AActor* Actor)
{
	if (Actor)
	{
		FowActors.Remove(Actor);
	}
}

bool AFogOfWarManager::IsActorCurrentlyInSight(AActor* Actor)
{
	//Find actor position
	if (!Actor) return false;
	FVector position = Actor->GetActorLocation();

	float dividend = 100.0f / SamplesPerMeter;
	uint32 halfTextureSize = TextureSize / 2;
	int posX = (int)(position.X / dividend) + halfTextureSize;
	int posY = (int)(position.Y / dividend) + halfTextureSize;

	return CurrentlyInSight.Contains(FVector2D(posX, posY));
}

bool AFogOfWarManager::GetIsBlurEnabled() 
{
	return bIsBlurEnabled;
}

void AFogOfWarManager::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
	if (Texture && Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			UpdateTextureRegionsData,
			FUpdateTextureRegionsData*, RegionData, RegionData,
			bool, bFreeData, bFreeData,
			{
				for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
				{
					int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
					if (RegionData->MipIndex >= CurrentFirstMip)
					{
						RHIUpdateTexture2D(
							RegionData->Texture2DResource->GetTexture2DRHI(),
							RegionData->MipIndex - CurrentFirstMip,
							RegionData->Regions[RegionIndex],
							RegionData->SrcPitch,
							RegionData->SrcData
							+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
							+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
							);
					}
				}
				if (bFreeData)
				{
					FMemory::Free(RegionData->Regions);
					FMemory::Free(RegionData->SrcData);
				}
				delete RegionData;
			});
	}
}