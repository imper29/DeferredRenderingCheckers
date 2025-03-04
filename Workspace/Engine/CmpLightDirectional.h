#pragma once
#include "float3.h"
#include "EntityComponent.h"

struct CmpLightDirectional : public EntityComponent {
	uint64_t layers;
	float3 color;

	CmpLightDirectional(const float3& color, const uint64_t& layers = 1) : color(color), layers(layers) {}
};
