#include "chomper/snake.hpp"
#include "chomper/world_space.hpp"
#include <imgui.h>
#include <klib/fixed_string.hpp>

namespace chomper {
namespace {
constexpr auto moveSpeed_v = kvf::Seconds{0.135f};
constexpr auto snakeBodyColor_v = kvf::Color(glm::vec4{0.f, 0.6f, 1.f, 1.f});
constexpr auto oppositeHeading_v = klib::EnumArray<Heading, Heading>{Heading::West, Heading::South, Heading::East, Heading::North};
constexpr auto headingToDir_v = klib::EnumArray<Heading, glm::vec2>{glm::vec2{1.f, 0.f}, glm::vec2{0.f, 1.f}, glm::vec2{-1.f, 0.f}, glm::vec2{0.f, -1.f}};
} // namespace

Snake::Snake() {
	// initialize snake body
	grow();
}

void Snake::tick(kvf::Seconds dt) {
	m_moveTimer += dt;

	if (m_moveTimer >= moveSpeed_v) {
		m_moveTimer = {};

		// no body, no movement
		if (m_instances.empty()) {
			return;
		}

		if (!m_headingQueue.empty()) {
			m_heading = m_headingQueue.front();
			m_headingQueue.erase(m_headingQueue.begin());
		}

		grow();

		if (m_shouldPop) {
			popTail();
		}
	}

	m_quads.instances.clear();
	m_quads.instances.reserve(m_instances.size());
	std::ranges::copy(m_instances, std::back_inserter(m_quads.instances));
}

void Snake::draw(le::IRenderer& renderer) const {
	m_quads.draw(renderer);
}

void Snake::popTail() {
	if (!m_instances.empty()) {
		m_instances.erase(m_instances.begin());
	}
}

void Snake::grow() {
	le::RenderInstance instance{};
	instance.tint = snakeBodyColor_v;
	// no reason to move on initialization
	if (!m_instances.empty()) {
		instance.transform.position = worldSpace::gridToWorld(worldSpace::worldToGrid(m_instances.back().transform.position) + headingToDir_v[m_heading]);
	}

	m_instances.push_back(instance);
}

void Snake::setHeading(Heading heading) {
	auto lastHeading = m_headingQueue.empty() ? m_heading : m_headingQueue.back();
	if (heading == m_heading || heading == oppositeHeading_v[lastHeading]) {
		return;
	}

	if (m_headingQueue.size() < 3) {
		m_log.debug("changing heading from {} to {}", headingName_v[m_heading], headingName_v[heading]);
		m_headingQueue.push_back(heading);
	}
}

void Snake::debugInspect() {
	ImGui::TextUnformatted("snake");
	ImGui::Separator();
	ImGui::TextUnformatted(klib::FixedString{"Heading: {}", headingName_v[m_heading]}.c_str());

	if (ImGui::TreeNode("heading queue")) {
		for (auto const heading : m_headingQueue) {
			ImGui::TextUnformatted(headingName_v[heading].data());
		}
		ImGui::TreePop();
	}
}
} // namespace chomper
