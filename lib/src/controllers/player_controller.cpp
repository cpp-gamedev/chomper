#include "chomper/controllers/player_controller.hpp"
#include <array>

namespace chomper {
using KeyDigital = le::input::action::KeyDigital;
using ActionValue = le::input::action::Value;

namespace {
struct TurnInputs {
	Heading heading{};
	std::vector<int> keys{};
};

auto const defaultTurnInputs = std::array{
	TurnInputs{.heading = Heading::East, .keys = {GLFW_KEY_RIGHT, GLFW_KEY_D}},
	TurnInputs{.heading = Heading::North, .keys = {GLFW_KEY_UP, GLFW_KEY_W}},
	TurnInputs{.heading = Heading::West, .keys = {GLFW_KEY_LEFT, GLFW_KEY_A}},
	TurnInputs{.heading = Heading::South, .keys = {GLFW_KEY_DOWN, GLFW_KEY_S}},
};
} // namespace

PlayerController::PlayerController(gsl::not_null<IListener*> listener, le::input::ActionMapping& mapping) : IController(listener) {
	createTurnActions();
	bindActions(mapping);
}

void PlayerController::tick(kvf::Seconds const /*dt*/) {}

void PlayerController::createTurnActions() {
	for (auto const& turnInputs : defaultTurnInputs) {
		auto& turn_action = m_turnActions[turnInputs.heading];
		for (auto const key : turnInputs.keys) {
			turn_action.push_back(std::make_unique<KeyDigital>(key));
		}
	}
}

void PlayerController::bindActions(le::input::ActionMapping& mapping) {
	for (auto heading = Heading{}; heading < Heading::COUNT_; heading = Heading(int(heading) + 1)) {
		// we're storing callbacks that capture 'this' into an external (non-owned) mapping.
		// the actual owner MUST be responsible for clearing these mappings when 'this' is destroyed.
		auto const onAction = [this, heading](ActionValue const v) {
			if (m_listener && v.get<bool>()) {
				m_listener->onSetHeading(heading);
			}
		};
		for (auto const& key : m_turnActions[heading]) {
			mapping.bind_action(key.get(), onAction);
		}
	}
}
} // namespace chomper
