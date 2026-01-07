#include "chomper/player.hpp"
#include "chomper/controllers/player_controller.hpp"

namespace chomper {
Player::Player(le::input::ScopedActionMapping& mapping) {
	createController(mapping);
}

void Player::tick(kvf::Seconds dt) {
	m_controller->tick(dt);
	m_snake.tick(dt);
}

void Player::draw(le::IRenderer& renderer) const {
	m_snake.draw(renderer);
}

void Player::debugInspect() {
	m_snake.debugInspect();
}

void Player::createController(le::input::ScopedActionMapping& mapping) {
	m_controller = std::make_unique<PlayerController>(this, mapping);
}

void Player::onSetHeading(Heading const heading) {
	m_snake.setHeading(heading);
}

} // namespace chomper
