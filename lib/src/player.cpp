#include "chomper/player.hpp"

namespace chomper {
Player::Player(le::input::ScopedActionMapping& mapping) {
	createController(mapping);
}

void Player::tick(kvf::Seconds dt) {
	m_controller->tick(dt);

	// ImGui debug window
	if (ImGui::Begin("Debug")) {
		ImGui::TextUnformatted(klib::FixedString{"Heading: {}", heading_name_v[m_heading]}.c_str());
	}
	ImGui::End();
}

void Player::render(le::IRenderer& /*renderer*/) const {
	// TODO
}

void Player::createController(le::input::ScopedActionMapping& mapping) {
	m_controller = std::make_unique<PlayerController>(this, mapping);
}

void Player::onSetHeading(Heading const heading) {
	if (heading == m_heading) {
		return;
	}
	m_log.debug("changing heading from {} to {}", heading_name_v[m_heading], heading_name_v[heading]);
	m_heading = heading;
}

} // namespace chomper