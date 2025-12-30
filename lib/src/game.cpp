#include "chomper/game.hpp"
#include "chomper/im_util.hpp"
#include <imgui.h>
#include <klib/fixed_string.hpp>
#include <array>

namespace chomper {
using ActionValue = le::input::action::Value;

Game::Game(gsl::not_null<Engine*> engine) : m_engine(engine), m_mapping(&engine->getInputRouter()) {
	createPlayer();
}

void Game::tick(kvf::Seconds const dt) {
	m_player->tick(dt);

	ImGui::SetNextWindowSize({300.0f, 200.0f}, ImGuiCond_Once);
	if (ImGui::Begin("Debug Inspect")) {
		debugInspectWindow();
	}
	ImGui::End();
}

void Game::render(le::IRenderer& renderer) const {
	m_player->render(renderer);
}

void Game::debugInspectWindow() {
	auto const inspectables = std::array{
		InspectItem{.inspectable = m_player.get(), .label = "Player"},
	};
	im_util::inspectAsTabs(inspectables);
}

void Game::bindActions() {
	// goBackKey is separated from PlayerController so that it works regardless of the type of controller in use.
	// this implies that all actions must share the same mapping to be active simultaneously.
	m_mapping.bind_action(&m_actions.goBackKey, [this](ActionValue const v) {
		if (v.get<bool>()) {
			onGoBack();
		}
	});
}

void Game::createPlayer() {
	// clear bindings that may point to dangling actions after existing player gets destroyed.
	m_mapping.clear_bindings();
	// rebind game actions.
	bindActions();
	// create the player, passing a reference of the logger and a reference of the input mapping to create its PlayerController.
	m_player = std::make_unique<Player>(m_mapping);
}

void Game::onGoBack() {
	m_log.debug("execute 'go back' action here");
}
} // namespace chomper
