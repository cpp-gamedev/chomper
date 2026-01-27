#pragma once
#include "chomper/viewport.hpp"
#include <glm/vec2.hpp>

namespace chomper {
constexpr auto worldSize_v = glm::ivec2{16};
constexpr auto tileSize_v = glm::ivec2{viewport_v.world_size} / worldSize_v;
} // namespace chomper
