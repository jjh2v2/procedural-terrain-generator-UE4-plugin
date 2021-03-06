// Copyright (C) 2019 Pedro Montoto García - All Rights Reserved

#include "PerlinNoiseAdderFilter.h"

DEFINE_LOG_CATEGORY(LandscapeFilter);

/**
 * References: 
 * https://mrl.nyu.edu/~perlin/paper445.pdf
 * https://github.com/sol-prog/Perlin_Noise/blob/master/PerlinNoise.cpp
 */
class PerlinNoiseGenerator
{
	// The permutation vector
	TArray<uint32> Permutation;
	FRandomStream* RandomStream;

public:
	// Initialize with the reference values for the permutation vector
	PerlinNoiseGenerator()
	{
		PerlinNoiseGenerator(0);
	}
	// Generate a new permutation vector based on the value of seed
	PerlinNoiseGenerator(FRandomStream* InRandomStream)
	{
		RandomStream = InRandomStream;
		Permutation.Init(0, 256);

		for (int32 i = 0; i < Permutation.Num(); i++)
		{
			Permutation[i] = i;
		}

		Permutation.Sort([this](uint32 A, uint32 B) { return RandomStream->RandRange(-1, 1); });
		Permutation.Append(TArray<uint32>(Permutation));
	}

	// Get a noise value, for 2D images z can have any value
	double Noise(double x, double y, double z)
	{
		// Find the unit cube that contains the point
		int X = (int)floor(x) & 255;
		int Y = (int)floor(y) & 255;
		int Z = (int)floor(z) & 255;

		// Find relative x, y,z of point in cube
		x -= floor(x);
		y -= floor(y);
		z -= floor(z);

		// Compute fade curves for each of x, y, z
		double u = Fade(x);
		double v = Fade(y);
		double w = Fade(z);

		// Hash coordinates of the 8 cube corners
		int A = Permutation[X] + Y;
		int AA = Permutation[A] + Z;
		int AB = Permutation[A + 1] + Z;
		int B = Permutation[X + 1] + Y;
		int BA = Permutation[B] + Z;
		int BB = Permutation[B + 1] + Z;

		// Add blended results from 8 corners of cube
		double LerpA = FMath::Lerp(FMath::Lerp(Grad(Permutation[AA], x, y, z), Grad(Permutation[BA], x - 1, y, z), u), FMath::Lerp(Grad(Permutation[AB], x, y - 1, z), Grad(Permutation[BB], x - 1, y - 1, z), u), v);
		double LerpB = FMath::Lerp(FMath::Lerp(Grad(Permutation[AA + 1], x, y, z - 1), Grad(Permutation[BA + 1], x - 1, y, z - 1), u), FMath::Lerp(Grad(Permutation[AB + 1], x, y - 1, z - 1), Grad(Permutation[BB + 1], x - 1, y - 1, z - 1), u), v);
		return FMath::Lerp(LerpA, LerpB, w);
	}

private:
	double Fade(double t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	double Grad(int hash, double x, double y, double z)
	{
		int h = hash & 15;
		// Convert lower 4 bits of hash into 12 gradient directions
		double u = h < 8 ? x : y;
		double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
};

bool UPerlinNoiseAdderFilter::ModifyHeightMap(TArray<uint16>* RawHeightMapData, FBounds Bounds, FRandomStream* RandomStream)
{
	PerlinNoiseGenerator Perlin(RandomStream);
	for (int j = Bounds.MinY; j <= Bounds.MaxY; j++)
	{
		for (int i = Bounds.MinX; i <= Bounds.MaxX; i++)
		{
			int CurrentIndex = (j - Bounds.MinY) * (Bounds.MaxX - Bounds.MinX + 1) + (i - Bounds.MinX);

			double YProp = (double)(j - Bounds.MinY) / (double)(Bounds.MaxY - Bounds.MinY);
			double XProp = (double)(i - Bounds.MinX) / (double)(Bounds.MaxX - Bounds.MinX);
			double NoiseVal = Perlin.Noise(XProp * NoiseScale.X, YProp * NoiseScale.Y, 0);

			(*RawHeightMapData)[CurrentIndex] = ULandscapeFilter::SafeOverflowAdd((*RawHeightMapData)[CurrentIndex], FMath::RoundToInt(NoiseVal * NoiseScale.Z));
		}
	}

	return true;
}
