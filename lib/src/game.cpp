#include "chomper/game.hpp"
#include "chomper/controllers/player_controller.hpp"
#include <imgui.h>
#include <klib/fixed_string.hpp>

namespace chomper {
using ActionValue = le::input::action::Value;

Game::Game(gsl::not_null<Engine*> engine) : m_engine(engine), m_mapping(&engine->get_input_router()) { create_controller(); }

void Game::tick(kvf::Seconds const /*dt*/) {
	if (ImGui::Begin("Debug")) { ImGui::TextUnformatted(klib::FixedString{"Heading: {}", heading_name_v[m_heading]}.c_str()); }
	ImGui::End();
}

void Game::render(le::IRenderer& /*renderer*/) const {}

void Game::on_set_heading(Heading const heading) {
	if (heading == m_heading) { return; }
	m_log.debug("changing heading from {} to {}", heading_name_v[m_heading], heading_name_v[heading]);
	m_heading = heading;
}

void Game::create_controller() {
	// clear bindings that may point to dangling actions after existing controller gets destroyed.
	m_mapping.clear_bindings();
	// rebind game actions.
	bind_actions();

	// can technically be any type of IController, eg auto-play.
	// PlayerController stores 'this', so Game must remain address-stable. this is why it inherits from Pinned.
	m_controller = std::make_unique<PlayerController>(this, m_mapping);
}

void Game::bind_actions() {
	// go_back_key is separated from PlayerController so that it works regardless of the type of controller in use.
	// this implies that all actions must share the same mapping to be active simultaneously.
	m_mapping.bind_action(&m_actions.go_back_key, [this](ActionValue const v) {
		if (v.get<bool>()) { on_go_back(); }
	});
}

void Game::on_go_back() { m_log.debug("execute 'go back' action here"); }
} // namespace chomper
