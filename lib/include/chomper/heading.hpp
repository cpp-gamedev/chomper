#pragma once
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <klib/enum_array.hpp>

namespace chomper {
// represents a direction.
enum class Heading : std::int8_t {
	East,
	North,
	West,
	South,
	COUNT_
};

constexpr auto headingName_v = klib::EnumArray<Heading, std::string_view>{"East", "North", "West", "South"};
constexpr auto oppositeHeading_v = klib::EnumArray<Heading, Heading>{Heading::West, Heading::South, Heading::East, Heading::North};
constexpr auto headingToDir_v = klib::EnumArray<Heading, glm::ivec2>{glm::ivec2{1, 0}, glm::ivec2{0, 1}, glm::ivec2{-1, 0}, glm::ivec2{0, -1}};
constexpr klib::EnumArray<Heading, float> headingToRot_v{glm::radians(90.f), glm::radians(0.f), glm::radians(270.f), glm::radians(180.f)};

} // namespace chomper