#include "EntityRegistry.h"

EntityRegistry::EntityRegistry() : m_Metadatas(new EntityMetadata[1]), m_MetadatasCap(1), m_MetadatasFree(0), m_MetadatasHead(0) {

}
EntityRegistry::~EntityRegistry() noexcept {
    uint32_t index = m_MetadatasHead;
    while (index) {
        //Destroy current entity.
        EntityMetadata* meta = m_Metadatas + index;
        for (uint32_t i = 0; i < meta->m_ComponentsCount; ++i)
            delete meta->m_Components[i];
        delete[] meta->m_Components;
        //Select next entity.
        index = meta->m_Next;
    }
    //Destroy metadata buffer.
    delete[] m_Metadatas;
}

EntityId EntityRegistry::Create() {
    //Ensure entity capacity.
    if (!m_MetadatasFree) {
        uint32_t nMetadatasCap = (m_MetadatasCap + 1u) * 2u;
        EntityMetadata* nMetadatas = new EntityMetadata[nMetadatasCap];
        memcpy(nMetadatas, m_Metadatas, sizeof(EntityMetadata) * m_MetadatasCap);
        delete[] m_Metadatas;
        //Insert unallocated entities into freelist.
        for (uint32_t i = m_MetadatasCap; i < nMetadatasCap; ++i)
            nMetadatas[i].m_Next = i + 1u;
        nMetadatas[nMetadatasCap - 1u].m_Next = 0u;
        m_MetadatasFree = m_MetadatasCap;
        //Assign buffers to reallocated buffers.
        m_MetadatasCap = nMetadatasCap;
        m_Metadatas = nMetadatas;
    }
    //Create entity.
    uint32_t index = m_MetadatasFree;
    EntityMetadata* meta = m_Metadatas + index;
    m_MetadatasFree = meta->m_Next;
    //Insert entity into list.
    if (m_MetadatasHead) {
        EntityMetadata* head = m_Metadatas + m_MetadatasHead;
        meta->m_Prev = 0u;
        meta->m_Next = m_MetadatasHead;
        head->m_Prev = index;
        m_MetadatasHead = index;
    }
    else {
        meta->m_Prev = 0u;
        meta->m_Next = m_MetadatasHead;
        m_MetadatasHead = index;
    }
    return { index, meta->m_Version };
}
bool EntityRegistry::Exists(EntityId entity) {
    if (entity.index >= m_MetadatasCap)
        return false;
    return m_Metadatas[entity.index].m_Version == entity.version;
}
bool EntityRegistry::Destroy(EntityId entity) noexcept {
    EntityMetadata* meta = GetMetadata(entity);
    if (!meta)
        return false;
    //Destroy entity components.
    for (uint32_t i = 0; i < meta->m_ComponentsCount; ++i)
        delete meta->m_Components[i];
    delete[] meta->m_Components;
    meta->m_Components = nullptr;
    //Destroy entity.
    ++meta->m_Version;
    //Exclude entity from list.
    if (m_MetadatasHead == entity.index)
        m_MetadatasHead = meta->m_Next;
    if (meta->m_Next)
        m_Metadatas[meta->m_Next].m_Prev = meta->m_Prev;
    if (meta->m_Prev)
        m_Metadatas[meta->m_Prev].m_Next = meta->m_Next;
    //Include entity in freelist.
    m_Metadatas[entity.index].m_Next = m_MetadatasFree;
    m_MetadatasFree = entity.index;
    return true;
}
