#include "chomper/snake.hpp"
#include "chomper/world_size.hpp"
#include <imgui.h>
#include <klib/fixed_string.hpp>

namespace chomper {
namespace {
constexpr auto moveSpeed_v = kvf::Seconds{0.135f};
constexpr auto snakeBodyColor = kvf::Color(glm::vec4{0.f, 0.6f, 1.f, 1.f});
constexpr Heading oppositeHeading_v[]{Heading::West, Heading::South, Heading::East, Heading::North};
constexpr glm::vec2 headingToDir_v[]{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
} // namespace
Snake::Snake() {
	// initialize snake body
	grow();
}
void Snake::tick(kvf::Seconds dt) {
	m_moveTimer += dt;

	if (m_moveTimer >= moveSpeed_v) {
		m_moveTimer = {};

		if (!m_headingQueue.empty()) {
			m_heading = m_headingQueue.front();
			m_headingQueue.pop_front();
		}

		auto targetPosition = m_quads.back().transform.position + headingToDir_v[static_cast<uint8_t>(m_heading)] * tileSize_v;
		grow();
		move(targetPosition);

		if (m_shouldPop) {
			popTail();
		}
	}
}

void Snake::render(le::IRenderer& renderer) const {
	for (auto const& quad : m_quads) {
		quad.draw(renderer);
	}
}

void Snake::popTail() {
	if (!m_quads.empty()) {
		m_quads.pop_front();
	}
}

void Snake::grow() {
	auto quad = le::drawable::Quad{};
	quad.create({tileSize_v}, {}, snakeBodyColor);
	m_quads.push_back(quad);
}

void Snake::move(glm::vec2 position) {
	m_quads.back().transform.position = position;
}

void Snake::setHeading(Heading heading) {
	auto lastHeading = m_headingQueue.empty() ? m_heading : m_headingQueue.back();
	if (heading == m_heading || heading == oppositeHeading_v[static_cast<uint8_t>(lastHeading)]) {
		return;
	}

	if (m_headingQueue.size() < 3) {
		m_log.debug("changing heading from {} to {}", headingName_v[m_heading], headingName_v[heading]);
		m_headingQueue.push_back(heading);
	}
}

void Snake::debugInspect() {
	ImGui::TextUnformatted(klib::FixedString{"Heading: {}", headingName_v[m_heading]}.c_str());
}
} // namespace chomper