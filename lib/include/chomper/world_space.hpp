#pragma once
#include "chomper/world_size.hpp"
#include <glm/common.hpp>
#include <glm/vec2.hpp>

namespace chomper::worldSpace {
namespace {
auto const halfGridSize = glm::ceil(worldSize_v * 0.5f);

[[nodiscard]] constexpr auto toTileOffset(float const worldSize, float const tileSize) -> float {
	// only add half the tilesize when that axis is even
	return (static_cast<int>(worldSize) % 2 == 0) ? tileSize * 0.5f : 0.0f;
}

constexpr auto tileOffset = glm::vec2{toTileOffset(worldSize_v.x, tileSize_v.x), toTileOffset(worldSize_v.y, tileSize_v.y)};
} // namespace

constexpr auto gridToWorld(glm::vec2 gridPosition) {
	return ((gridPosition - halfGridSize) * tileSize_v) + tileOffset;
}
constexpr auto worldToGrid(glm::vec2 worldPosition) {
	return glm::floor((worldPosition - tileOffset) / tileSize_v) + halfGridSize;
}

constexpr auto isOutOfBounds(glm::vec2 gridPoint) {
	return gridPoint.x < 0 || gridPoint.y < 0 || gridPoint.x >= worldSize_v.x || gridPoint.y >= worldSize_v.y;
}
} // namespace chomper::worldSpace
