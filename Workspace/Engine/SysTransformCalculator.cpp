#include "SysTransformCalculator.h"

#include "CmpParent.h"
#include "CmpPosition.h"
#include "CmpRotation.h"
#include "CmpScale.h"
#include "CmpTransformLocalToWorld.h"

#include <unordered_map>
#include <vector>

template<>
struct std::hash<EntityId> {
	std::size_t operator()(const EntityId& k) const {
		return k.index_and_version * 9302639;//multiplied by a large prime number;
	}
};

struct transformHierarchyEntry {
	float4x4 matrix;
	EntityId entity;
	size_t parent;

	transformHierarchyEntry(const float4x4& matrix, const EntityId& entity, size_t parent) : matrix(matrix), entity(entity), parent(parent) {}
};
struct transformHierarchy {
	std::unordered_map<EntityId, size_t> indicies;
	std::vector<transformHierarchyEntry> entries;

private:
	size_t includeEntity(EntityRegistry& entities, const EntityId& entity, const float4x4& matrix) {
		//Include entity parent.
		auto parent = entities.GetComponent<CmpParent>(entity);
		auto parentIndex = SIZE_MAX;
		if (parent && entities.Exists(parent->parent)) {
			auto parentMatrix = entities.GetComponent<CmpTransformLocalToWorld>(parent->parent);
			if (parentMatrix) {
				parentIndex = includeEntity(entities, parent->parent, parentMatrix->value);
			}
		}
		//Include entity.
		auto [emplacement, success] = indicies.try_emplace(entity, entries.size());
		if (success) {
			entries.push_back(transformHierarchyEntry(matrix, entity, parentIndex));
		}
		return emplacement->second;
	}
public:
	void refreshMatrices(EntityRegistry& entities) {
		for (auto& entry : entries) {
			if (entry.parent != SIZE_MAX) {
				entry.matrix = entries[entry.parent].matrix * entry.matrix;
			}
		}
		for (auto& entry : entries) {
			entities.GetComponent<CmpTransformLocalToWorld>(entry.entity)->value = entry.matrix;
		}
	}
	transformHierarchy(EntityRegistry& entities) {
		auto entity = entities.First();
		while (entity.index) {
			auto matrix = entities.GetComponent<CmpTransformLocalToWorld>(entity);
			if (matrix) {
				includeEntity(entities, entity, matrix->value);
			}
			entity = entities.Next(entity);
		}
	}
};

SysTransformCalculator::SysTransformCalculator(Scene& scene) : SceneSystem(scene) {

}

void SysTransformCalculator::OnEvtReceived(Scene::EvtUpdate& evt) {
	//Generate local matricies in local to world component.
	{
		auto entity = scene.entities.First();
		while (entity.index) {
			auto localToWorld = scene.entities.GetComponent<CmpTransformLocalToWorld>(entity);
			auto position = scene.entities.GetComponent<CmpPosition>(entity);
			auto rotation = scene.entities.GetComponent<CmpRotation>(entity);
			auto scale = scene.entities.GetComponent<CmpScale>(entity);
			if (localToWorld && (position || rotation || scale)) {
				localToWorld->value = float4x4::identity();
				if (position)
					localToWorld->value = localToWorld->value * float4x4::translate(position->value);
				if (rotation)
					localToWorld->value = localToWorld->value * float4x4::rotate(rotation->value);
				if (scale)
					localToWorld->value = localToWorld->value * float4x4::scale(scale->value);
			}
			entity = scene.entities.Next(entity);
		}
	}
	//Generate local to world matrices.
	transformHierarchy hierarchy = transformHierarchy(scene.entities);
	hierarchy.refreshMatrices(scene.entities);
}
