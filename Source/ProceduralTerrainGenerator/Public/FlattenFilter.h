// Copyright (C) 2019 Pedro Montoto García - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LandscapeFilter.h"
#include "FlattenFilter.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PROCEDURALTERRAINGENERATOR_API UFlattenFilter : public ULandscapeFilter
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	uint16 Height = ULandscapeFilter::ZeroInLandscape; // "zero" height for Landscapes: this aligns the terrain to the actor transform

	// Inherited via ULandscapeFilter
	virtual bool ModifyHeightMap(TArray<uint16>* RawHeightMapData, FBounds Bounds, FRandomStream* RandomStream) override;
};
