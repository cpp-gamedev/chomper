#include "chomper/runtimes/game.hpp"
#include "chomper/collectible.hpp"
#include "chomper/im_util.hpp"
#include "chomper/runtimes/entrypoint.hpp"
#include "chomper/world_size.hpp"
#include "chomper/world_space.hpp"
#include <le2d/random.hpp>
#include <algorithm>

namespace chomper::runtime {
using ActionValue = le::input::action::Value;

Game::Game(gsl::not_null<Engine*> engine) : m_engine(engine), m_mapping(&engine->getInputRouter()) {
	createPlayer();
	m_world = std::make_unique<World>(m_engine);
	createCollectibleTexture();

	for (size_t i = 0; i < m_collectibleAmount; ++i) {
		spawnCollectible();
	}
}

void Game::tick(kvf::Seconds const dt) {
	m_player->tick(dt);

	collideCollectibles();

	ImGui::SetNextWindowSize({300.0f, 300.0f}, ImGuiCond_Once);
	if (ImGui::Begin("Debug Inspect")) {
		debugInspectWindow();
	}
	ImGui::End();

	// On death
	if (!m_player->getInfo().alive) {
		m_engine->setNextRuntime<runtime::Entrypoint>();
	}
}

void Game::render(le::IRenderer& renderer) const {
	m_world->draw(renderer);
	m_player->draw(renderer);
	for (auto const& collectible : m_collectibles) {
		collectible.draw(renderer);
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

void Game::createCollectibleTexture() {
	m_collectibleTexture = m_engine->getResources().load<le::ITexture>("images/apple.png");
}

void Game::spawnCollectible() {
	m_occupied.clear();
	for (auto const& seg : m_player->getSegments()) {
		auto p = worldSpace::worldToGrid(seg.transform.position);
		m_occupied.insert(static_cast<int>((p.y * worldSize_v.x) + p.x));
	}

	for (auto const& c : m_collectibles) {
		auto p = c.getGridPosition();
		m_occupied.insert(static_cast<int>((p.y * worldSize_v.x) + p.x));
	}

	auto emptyTiles = (worldSize_v.x * worldSize_v.y) - static_cast<float>(m_occupied.size());
	if (emptyTiles <= 0) {
		return;
	}

	auto random = m_random.next_int(0, static_cast<int>(emptyTiles - 1));

	auto count = 0;
	auto const width = static_cast<int>(worldSize_v.x);
	for (int index = 0; index < static_cast<int>(worldSize_v.x * worldSize_v.y); index++) {
		if (m_occupied.contains(index)) {
			continue;
		}
		if (count++ == random) {
			m_collectibles.emplace_back(*m_collectibleTexture, worldSpace::gridToWorld({index % width, index / width}));
			return;
		}
	}
}

void Game::collideCollectibles() {
	auto it = std::ranges::find_if(m_collectibles, [&](auto const& collectible) {
		return collectible.getGridPosition() == worldSpace::worldToGrid(m_player->getSegments().back().transform.position);
	});
	if (it != m_collectibles.end()) {
		m_collectibles.erase(it);
		m_player->grow();
		spawnCollectible();
	}
}

void Game::onGoBack() {
	m_log.debug("execute 'go back' action here");
}
} // namespace chomper::runtime
