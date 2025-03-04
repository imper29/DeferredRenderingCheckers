#pragma once
#include "float3.h"
#include "EntityComponent.h"

struct CmpPosition : public EntityComponent
{
	float3 value;

	CmpPosition() : value() {}
	CmpPosition(const float3& value) : value(value) {}
};
