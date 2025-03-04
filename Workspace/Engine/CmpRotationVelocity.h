#pragma once
#include "EntityComponent.h"
#include "float3.h"
struct CmpRotationVelocity : public EntityComponent {
	CmpRotationVelocity() : rotation() {}
	CmpRotationVelocity(float3 rotation) : rotation(rotation) {}

	float3 rotation;
};
