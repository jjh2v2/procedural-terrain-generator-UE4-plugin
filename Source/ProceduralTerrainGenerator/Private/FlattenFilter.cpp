// Copyright (C) 2019 Pedro Montoto García - All Rights Reserved

#include "FlattenFilter.h"

bool UFlattenFilter::ModifyHeightMap(TArray<uint16>* RawHeightMapData, FBounds Bounds, FRandomStream* RandomStream)
{
	RawHeightMapData->Init(Height, (Bounds.MaxX - Bounds.MinX + 1) * (Bounds.MaxY - Bounds.MinY + 1));

	return true;
}
