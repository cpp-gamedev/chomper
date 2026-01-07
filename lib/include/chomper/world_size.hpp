#pragma once
#include "chomper/viewport.hpp"
#include <glm/vec2.hpp>

namespace chomper {
constexpr auto worldSize_v = glm::vec2{15.f};
constexpr auto tileSize_v = viewport_v.world_size / worldSize_v;
} // namespace chomper
