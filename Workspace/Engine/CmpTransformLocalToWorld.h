#pragma once
#include "float4x4.h"
#include "EntityComponent.h"

struct CmpTransformLocalToWorld : public EntityComponent
{
	float4x4 value;

	CmpTransformLocalToWorld() : value(float4x4::identity()) {

	}
	CmpTransformLocalToWorld(const float4x4& value) : value(value) {

	}
};
