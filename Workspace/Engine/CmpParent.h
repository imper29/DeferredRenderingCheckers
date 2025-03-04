#pragma once
#include "EntityId.h"
#include "EntityComponent.h"

struct CmpParent : public EntityComponent
{
	EntityId parent;

	CmpParent(const EntityId& parent) : parent(parent) {}
};
