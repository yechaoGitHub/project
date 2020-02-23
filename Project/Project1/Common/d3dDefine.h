#pragma once
#include <stdint.h>

enum class ECubeFace :uint8_t 
{
	LEFT = 0,
	RIGHT,
	TOP,
	BOTTOM,
	FRONT,
	BACK
};

enum class EGraphicRootType : uint8_t
{
	CONSTANT_BUFFER,
	SRV,
	UAV,
};