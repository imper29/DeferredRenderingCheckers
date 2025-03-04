#pragma once
#include <cinttypes>
struct EntityId {
	union {
		struct {
			uint32_t index;
			uint32_t version;
		};
		uint64_t index_and_version;
	};

	bool operator==(const EntityId& other) const {
		return index_and_version == other.index_and_version;
	}
};
