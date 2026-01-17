#include "chomper/player.hpp"
#include "chomper/controllers/player_controller.hpp"
#include "chomper/engine.hpp"
#include "chomper/world_size.hpp"
#include "chomper/world_space.hpp"
#include <algorithm>

namespace chomper {
namespace {
constexpr auto moveSpeed_v = kvf::Seconds{0.135f};
constexpr auto oppositeHeading_v = klib::EnumArray<Heading, Heading>{Heading::West, Heading::South, Heading::East, Heading::North};
constexpr auto headingToDir_v = klib::EnumArray<Heading, glm::vec2>{glm::vec2{1.f, 0.f}, glm::vec2{0.f, 1.f}, glm::vec2{-1.f, 0.f}, glm::vec2{0.f, -1.f}};
} // namespace

Player::Player(le::input::ScopedActionMapping& mapping, gsl::not_null<Engine const*> engine) : m_engine(engine) {
	createController(mapping);
	updateScoreText();
}

void Player::tick(kvf::Seconds dt) {
	if (!m_info.alive) {
		return;
	}

	m_controller->tick(dt);

	m_moveTimer += dt;

	if (m_moveTimer >= moveSpeed_v) {
		m_moveTimer = {};
		move();
	}
}

void Player::grow() {
	m_info.score++;
	updateScoreText();
	m_shouldPop = false;
}

bool Player::isCollidingWithSelf(glm::vec2 const targetGrid) const {
	if (m_snake.getSegments().empty()) {
		return false;
	}
	return std::ranges::any_of(m_snake.getSegments(), [targetGrid](le::RenderInstance const& s) {
		return worldSpace::worldToGrid(s.transform.position) == targetGrid;
	});
}

bool Player::isCollidingWithWall(glm::vec2 const targetGrid) const {
	if (m_snake.getSegments().empty()) {
		return false;
	}
	return worldSpace::isOutOfBounds(targetGrid);
}

void Player::move() {
	// no body, no movement
	if (m_snake.getSegments().empty()) {
		return;
	}

	if (!m_headingQueue.empty()) {
		m_heading = m_headingQueue.front();
		m_headingQueue.erase(m_headingQueue.begin());
	}

	auto const targetGrid = worldSpace::worldToGrid(m_snake.getSegments().back().transform.position) + headingToDir_v[m_heading];
	if (isCollidingWithSelf(targetGrid) || isCollidingWithWall(targetGrid)) {
		if (m_graceMove) {
			m_info.alive = false;
		} else {
			m_graceMove = true;
		}

		return; // return so you don't move when you collide
	}

	m_snake.grow(m_heading);

	if (m_shouldPop) {
		m_snake.popTail();
	}

	m_shouldPop = true;	 // reset shouldPop
	m_graceMove = false; // reset graceMove
}

void Player::updateScoreText() {
	static constexpr auto textParams_v = le::drawable::Text::Params{
		.height = le::TextHeight{16},
	};

	m_scoreText.set_string(m_engine->getResources().getMainFont(), std::format("Score: {}", m_info.score), textParams_v);
	m_scoreText.transform.position = worldSpace::gridToWorld({0, worldSize_v.y - 1}) + glm::vec2{m_scoreText.get_size().x / 2, 0};
}

void Player::draw(le::IRenderer& renderer) const {
	m_snake.draw(renderer);
	m_scoreText.draw(renderer);
}

void Player::debugInspect() {
	ImGui::TextUnformatted(klib::FixedString{"Heading: {}", headingName_v[m_heading]}.c_str());

	if (ImGui::TreeNode("heading queue")) {
		for (auto const heading : m_headingQueue) {
			ImGui::TextUnformatted(headingName_v[heading].data());
		}
		ImGui::TreePop();
	}
}

void Player::createController(le::input::ScopedActionMapping& mapping) {
	m_controller = std::make_unique<PlayerController>(this, mapping);
}

void Player::onSetHeading(Heading const heading) {
	auto lastHeading = m_headingQueue.empty() ? m_heading : m_headingQueue.back();
	if (heading == m_heading || heading == oppositeHeading_v[lastHeading]) {
		return;
	}

	if (m_headingQueue.size() < 3) {
		m_log.debug("changing heading from {} to {}", headingName_v[m_heading], headingName_v[heading]);
		m_headingQueue.push_back(heading);
	}
}

} // namespace chomper
