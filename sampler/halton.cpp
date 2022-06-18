#include "halton.h"

constexpr uint32 primeTable[] = { 2, 3, 5, 7, 11,
13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89,
97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251,
257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347,
349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433,
439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523,
541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619,
631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727,
733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827,
829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937,
941, 947, 953, 967, 971, 977, 983, 991, 997 };

constexpr uint32 haltonMaxDimension = sizeof(primeTable) / sizeof(uint32);
constexpr uint32 samplerMaxResolution = 128;

constexpr uint32 sampleStrideX = 128, sampleStrideXExp = 7;
constexpr uint32 sampleStrideY = 243, sampleStrideExp = 5;
constexpr uint32 sampleStride = sampleStrideX * sampleStrideY;

constexpr uint32 gcdExpX = 59, gcdExpY = 131;


HaltonSampler::HaltonSampler(rtti::Struct* pStruct, uint32 samplePerPixel) :
	Sampler(pStruct, samplePerPixel),internalIndex(0) {
	al_assert(pStruct->GetSize() < haltonMaxDimension, "HaltonSampler::HaltonSampler : the size of the sturcture to sample"
		" should not be greater than {}.Struct {}'s size is {}", haltonMaxDimension, pStruct->GetTypeName() ,pStruct->GetSize());
}

float GeneralInverseDigits(uint32 index,uint32 base) {
	float invBase = 1.f / (float)base;
	float res = 0.f;
	float accInvBase = 1.f;
	while (index != 0) {
		uint32 digit = index % base;
		index = index / base;
		res = res * (float)base + (float)digit;
		accInvBase *= invBase;
	}
	return res * accInvBase;
}

uint32 InverseBits32(uint32 index) {
	index = (index & 0x0000ffff) << 16 | (index & 0xffff0000) >> 16;
	index = (index & 0x00ff00ff) << 8  | (index & 0xff00ff00) >> 8 ;
	index = (index & 0x0f0f0f0f) << 4  | (index & 0xf0f0f0f0) >> 4 ;
	index = (index & 0x33333333) << 2  | (index & 0xcccccccc) >> 2 ;
	index = (index & 0x55555555) << 1  | (index & 0xaaaaaaaa) >> 1 ;
	return index;
}


float HaltonSampler::SampleDimension(uint32 dimension) {
	if (dimension == 0) {
		return (float)InverseBits32(internalIndex / sampleStrideX) * 0x1p-32f;
	}
	else if (dimension == 1) {
		return GeneralInverseDigits(internalIndex / sampleStrideY, primeTable[dimension]);
	}
	else {
		return GeneralInverseDigits(internalIndex, primeTable[dimension]);
	}
}


static uint32 InverseDigits(uint32 num,uint32 base,uint32 exp) {
	uint32 res = 0;
	al_for(i,0,exp) {
		uint32 digit = num % base;
		num = num / base;
		res = res * base + digit;
	}
	return res;
}

uint32 HaltonSampler::GetInternalIndex(uint32 sampleIndex) {
	uint32 p[2] = { pixelX % samplerMaxResolution,pixelY % samplerMaxResolution };
	
	uint32 indexOffset = InverseDigits(p[0], 2, 7) * sampleStrideY
		* gcdExpY;
	indexOffset += InverseDigits(p[1], 3, 7) * sampleStrideX
		* gcdExpY;
	indexOffset %= sampleStride;

	return indexOffset + sampleIndex * sampleStride;
}

bool HaltonSampler::NextSample() {
	currentSampleIndex++;
	if (currentSampleIndex > samplePerPixel)
		return false;
	
	internalIndex = GetInternalIndex(currentSampleIndex);
	
	al_for(i, 0, pStruct->GetSize()) {
		sampledData[i] = SampleDimension(i);
	}
	return true;
}

void HaltonSampler::NextPixel(uint32 x,uint32 y) {
	currentSampleIndex = 0;
	pixelX = x, pixelY = y;
	internalIndex = GetInternalIndex(currentSampleIndex);
}