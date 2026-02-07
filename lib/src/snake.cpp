#include "chomper/snake.hpp"
#include "chomper/theme.hpp"
#include "chomper/world_size.hpp"
#include "chomper/world_space.hpp"
#include <imgui.h>
#include <klib/fixed_string.hpp>
#include <le2d/render_instance.hpp>

namespace chomper {
Snake::Snake() {
	le::RenderInstance instance{};
	instance.tint = theme::snakeBodyColor_v;
	instance.transform.position = worldSpace::gridToWorld({0, 0.5f * worldSize_v.y});
	m_instances.push_back(instance);
	while (m_instances.size() < m_baseSize) {
		grow({});
	}
}

void Snake::draw(le::IRenderer& renderer) const {
	m_quads.draw(renderer);
}

void Snake::grow(Heading heading) {
	le::RenderInstance instance{};
	instance.tint = theme::snakeBodyColor_v;
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
