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

auto const default_turn_inputs = std::array{
	TurnInputs{.heading = Heading::East, .keys = {GLFW_KEY_RIGHT, GLFW_KEY_D}},
	TurnInputs{.heading = Heading::North, .keys = {GLFW_KEY_UP, GLFW_KEY_W}},
	TurnInputs{.heading = Heading::West, .keys = {GLFW_KEY_LEFT, GLFW_KEY_A}},
	TurnInputs{.heading = Heading::South, .keys = {GLFW_KEY_DOWN, GLFW_KEY_S}},
};
} // namespace

PlayerController::PlayerController(gsl::not_null<IListener*> listener, le::input::ActionMapping& mapping) : IController(listener) {
	create_turn_actions();
	bind_actions(mapping);
}

void PlayerController::tick(kvf::Seconds const /*dt*/) {}

void PlayerController::create_turn_actions() {
	for (auto const& turn_inputs : default_turn_inputs) {
		auto& turn_action = m_turn_actions[turn_inputs.heading];
		for (auto const key : turn_inputs.keys) {
			turn_action.push_back(std::make_unique<KeyDigital>(key));
		}
	}
}

void PlayerController::bind_actions(le::input::ActionMapping& mapping) {
	for (auto heading = Heading{}; heading < Heading::COUNT_; heading = Heading(int(heading) + 1)) {
		// we're storing callbacks that capture 'this' into an external (non-owned) mapping.
		// the actual owner MUST be responsible for clearing these mappings when 'this' is destroyed.
		auto const on_action = [this, heading](ActionValue const v) {
			if (m_listener && v.get<bool>()) {
				m_listener->on_set_heading(heading);
			}
		};
		for (auto const& key : m_turn_actions[heading]) {
			mapping.bind_action(key.get(), on_action);
		}
	}
}
} // namespace chomper
