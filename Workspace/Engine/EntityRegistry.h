#pragma once
#include "EntityComponent.h"
#include "EntityId.h"
#include <memory>
struct EntityRegistry {
	EntityRegistry();
	~EntityRegistry() noexcept;

	EntityId Create();
	bool Exists(EntityId entity);
	bool Destroy(EntityId entity) noexcept;

	EntityId First() const noexcept {
		if (m_MetadatasHead)
			return { m_MetadatasHead, m_Metadatas[m_MetadatasHead].m_Version };
		return { 0u,0u };
	}
	EntityId Next(EntityId entity) const noexcept {
		if (m_Metadatas[entity.index].m_Version != entity.version)
			return { 0u, 0u };
		uint32_t index = m_Metadatas[entity.index].m_Next;
		return { index, m_Metadatas[index].m_Version };
	}
	EntityId Last() const noexcept {
		return { 0u,0u };
	}

	template<typename EntityComponentT>
	bool RemoveComponent(EntityId entity) noexcept {
		auto meta = GetMetadata(entity);
		if (!meta)
			return false;
		for (uint32_t i = 0; i < meta->m_ComponentsCount; ++i) {
			auto cmp = dynamic_cast<EntityComponentT*>(meta->m_Components[i]);
			if (cmp) {
				meta->m_Components[i] = meta->m_Components[--meta->m_ComponentsCount];
				delete cmp;
				return true;
			}
		}
		return false;
	}
	template<typename EntityComponentT>
	EntityComponentT* GetComponent(EntityId entity) noexcept {
		auto meta = GetMetadata(entity);
		if (!meta)
			return nullptr;
		for (uint32_t i = 0; i < meta->m_ComponentsCount; ++i) {
			auto cmp = dynamic_cast<EntityComponentT*>(meta->m_Components[i]);
			if (cmp) {
				return cmp;
			}
		}
		return nullptr;
	}
	template<typename EntityComponentT, typename ...ArgsT>
	EntityComponentT* GetOrAddComponent(EntityId entity, ArgsT... args) {
		auto meta = GetMetadata(entity);
		if (!meta)
			return nullptr;
		for (uint32_t i = 0; i < meta->m_ComponentsCount; ++i) {
			auto cmp = dynamic_cast<EntityComponentT*>(meta->m_Components[i]);
			if (cmp) {
				return cmp;
			}
		}
		if (meta->m_ComponentsCount + 1u >= meta->m_ComponentsCap) {
			uint32_t nComponentsCap = (meta->m_ComponentsCap + 1u) * 2u;
			EntityComponent** nComponents = new EntityComponent * [nComponentsCap];
			memcpy(nComponents, meta->m_Components, sizeof(EntityComponent*) * meta->m_ComponentsCount);
			delete[] meta->m_Components;
			meta->m_Components = nComponents;
			meta->m_ComponentsCap = nComponentsCap;
		}
		EntityComponentT* cmp = new EntityComponentT(args...);
		meta->m_Components[meta->m_ComponentsCount++] = cmp;
		return cmp;
	}

private:
	struct EntityMetadata {
		EntityComponent** m_Components = nullptr;
		uint32_t m_ComponentsCount = 0u;
		uint32_t m_ComponentsCap = 0u;
		uint32_t m_Version = 0u;
		uint32_t m_Next = 0u;
		uint32_t m_Prev = 0u;
	};
	EntityMetadata* GetMetadata(EntityId entity) {
		if (entity.index >= m_MetadatasCap)
			return nullptr;
		auto metadata = m_Metadatas + entity.index;
		if (metadata->m_Version != entity.version)
			return nullptr;
		return metadata;
	}

	EntityMetadata* m_Metadatas;
	uint32_t m_MetadatasHead;
	uint32_t m_MetadatasFree;
	uint32_t m_MetadatasCap;
};
