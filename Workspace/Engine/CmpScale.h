#pragma once
#include "float3.h"
#include "EntityComponent.h"

struct CmpScale : public EntityComponent
{
	float3 value;

	CmpScale() : value() {}
	CmpScale(const float3& value) : value(value) {}
};
