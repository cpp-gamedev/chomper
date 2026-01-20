#include "chomper/runtimes/game.hpp"
#include "chomper/im_util.hpp"
#include "chomper/runtimes/entrypoint.hpp"
#include "chomper/world_space.hpp"
#include <le2d/random.hpp>
#include <algorithm>

namespace chomper::runtime {
using ActionValue = le::input::action::Value;

Game::Game(gsl::not_null<Engine*> engine) : m_engine(engine), m_mapping(&engine->getInputRouter()) {
	createPlayer();
	m_world = std::make_unique<World>(m_engine);

	createCollectibles();

	m_collectibles->spawn(*m_player);

	m_countdown.emplace(&engine->getResources().getMainFont());
}

void Game::tick(kvf::Seconds const dt) {
	ImGui::SetNextWindowSize({300.0f, 300.0f}, ImGuiCond_Once);
	if (ImGui::Begin("Debug Inspect")) {
		debugInspectWindow();
	}
	ImGui::End();

	if (m_countdown) {
		m_countdown->tick(dt);
		if (m_countdown->getRemain() <= 0s) {
			m_countdown.reset();
		}
		return;
	}

	m_player->tick(dt);

	collideCollectibles();

	// On death
	if (!m_player->getInfo().alive) {
		m_engine->setNextRuntime<runtime::Entrypoint>();
	}
}

void Game::render(le::IRenderer& renderer) const {
	m_world->draw(renderer);
	m_collectibles->draw(renderer);
	m_player->draw(renderer);
	if (m_countdown) {
		m_countdown->draw(renderer);
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

void Game::createCollectibles() {
	m_collectibleTexture = m_engine->getResources().load<le::ITexture>("images/apple.png");
	m_collectibles = std::make_unique<Collectibles>(*m_collectibleTexture);
}

void Game::collideCollectibles() {
	auto it = std::ranges::find_if(m_collectibles->getInstances(), [&](auto const& collectible) {
		return worldSpace::worldToGrid(collectible.transform.position) == worldSpace::worldToGrid(m_player->getSegments().back().transform.position);
	});
	if (it == m_collectibles->getInstances().end()) {
		return;
	}

	m_player->grow();
	m_collectibles->eraseInstance(static_cast<std::size_t>(std::distance(m_collectibles->getInstances().begin(), it)));
	m_collectibles->spawn(*m_player);
}

void Game::onGoBack() {
	m_log.debug("execute 'go back' action here");
}
} // namespace chomper::runtime
