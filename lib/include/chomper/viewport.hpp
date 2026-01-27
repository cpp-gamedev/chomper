#pragma once
#include <le2d/viewport.hpp>

namespace chomper {
constexpr auto viewport_v = le::viewport::Letterbox{.world_size = glm::ivec2{800}};
} // namespace chomper
