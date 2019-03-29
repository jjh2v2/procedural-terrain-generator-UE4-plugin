// Copyright (C) 2019 Pedro Montoto García - All Rights Reserved

#include "PerlinNoiseAdderFilter.h"
#include "LandscapeComponent.h"

UPerlinNoiseAdderFilter::UPerlinNoiseAdderFilter(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{

}

bool UPerlinNoiseAdderFilter::ApplyFilter(ALandscape * Landscape)
{
	for (ULandscapeComponent* LandscapeComponent : Landscape->LandscapeComponents)
	{
	}
	return false;
}
