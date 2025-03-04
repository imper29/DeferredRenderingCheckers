#pragma once
#include "quaternion.h"
#include "EntityComponent.h"

struct CmpRotation : public EntityComponent
{
	quaternion value;

	CmpRotation() : value(quaternion::identity()) {}
	CmpRotation(const quaternion& value) : value(value) {}
};
