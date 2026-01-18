#include "chomper/runtimes/game.hpp"
#include "chomper/im_util.hpp"
#include "chomper/runtimes/entrypoint.hpp"
#include <array>

namespace chomper::runtime {
namespace {
constexpr auto countdownParams_v = le::drawable::Text::Params{
	.height = le::TextHeight{60},
};
}
using ActionValue = le::input::action::Value;

Game::Game(gsl::not_null<Engine*> engine) : m_engine(engine), m_mapping(&engine->getInputRouter()) {
	createPlayer();
	m_world = std::make_unique<World>(m_engine);

	m_countdownText.set_string(engine->getResources().getMainFont(), "3", countdownParams_v);
}

void Game::tick(kvf::Seconds const dt) {
	ImGui::SetNextWindowSize({300.0f, 300.0f}, ImGuiCond_Once);
	if (ImGui::Begin("Debug Inspect")) {
		debugInspectWindow();
	}
	ImGui::End();

	if (m_countdown.count() > 0) {
		m_countdown -= dt;
		m_countdownText.set_string(m_engine->getResources().getMainFont(), std::format("{}", static_cast<int>(m_countdown.count() + 1)), countdownParams_v);
		return;
	}

	m_player->tick(dt);

	// On death
	if (!m_player->getInfo().alive) {
		m_engine->setNextRuntime<runtime::Entrypoint>();
	}
}

void Game::render(le::IRenderer& renderer) const {
	m_world->draw(renderer);
	m_player->draw(renderer);
	if (m_countdown.count() > 0) {
		m_countdownText.draw(renderer);
	}
}

void Game::debugInspectWindow() {
	auto const inspectItems = std::array{
		InspectItem{.inspector = m_player.get(), .label = "Player"},
		InspectItem{.inspector = &m_engine->getResources(), .label = "Resources"},
		InspectItem{.inspector = m_engine, .label = "Engine"},
	};
	im_util::inspectAsTabs(inspectItems);
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
	m_player = std::make_unique<Player>(m_mapping, m_engine);
}

void Game::onGoBack() {
	m_log.debug("execute 'go back' action here");
}
} // namespace chomper::runtime
