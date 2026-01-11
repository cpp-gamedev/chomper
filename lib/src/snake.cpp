#include "chomper/snake.hpp"
#include "chomper/world_space.hpp"
#include <imgui.h>
#include <klib/fixed_string.hpp>

namespace chomper {
namespace {
constexpr auto snakeBodyColor_v = kvf::Color(glm::vec4{0.f, 0.6f, 1.f, 1.f});
constexpr auto headingToDir_v = klib::EnumArray<Heading, glm::vec2>{glm::vec2{1.f, 0.f}, glm::vec2{0.f, 1.f}, glm::vec2{-1.f, 0.f}, glm::vec2{0.f, -1.f}};
} // namespace

Snake::Snake() {
	while (m_instances.size() < m_baseSize) {
		grow({});
	}
}

void Snake::draw(le::IRenderer& renderer) const {
	m_quads.draw(renderer);
}

void Snake::grow(Heading heading) {
	le::RenderInstance instance{};
	instance.tint = snakeBodyColor_v;
	// no reason to move on initialization
	if (!m_instances.empty()) {
		instance.transform.position = worldSpace::gridToWorld(worldSpace::worldToGrid(m_instances.back().transform.position) + headingToDir_v[heading]);
	}

	m_instances.push_back(instance);

	syncQuads();
}

void Snake::popTail() {
	if (m_instances.empty()) {
		return;
	}
	m_instances.erase(m_instances.begin());
	syncQuads();
}

void Snake::syncQuads() {
	m_quads.instances.clear();
	m_quads.instances.reserve(m_instances.size());
	std::ranges::copy(m_instances, std::back_inserter(m_quads.instances));
}

} // namespace chomper
