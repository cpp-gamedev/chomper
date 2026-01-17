#include "chomper/runtimes/game.hpp"
#include "chomper/collectible.hpp"
#include "chomper/im_util.hpp"
#include "chomper/runtimes/entrypoint.hpp"
#include "chomper/world_size.hpp"
#include "chomper/world_space.hpp"
#include "glm/ext/vector_float2.hpp"
#include "le2d/resource/texture.hpp"
#include <array>
#include <memory>
#include <random>
#include <unordered_set>
#include <vector>

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
	std::unordered_set<int> occupied;
	for (auto const& seg : m_player->getSegments()) {
		auto p = worldSpace::worldToGrid(seg.transform.position);
		occupied.insert(static_cast<int>((p.y * worldSize_v.x) + p.x));
	}

	for (auto const& c : m_collectibles) {
		auto p = c.getGridPosition();
		occupied.insert(static_cast<int>((p.y * worldSize_v.x) + p.x));
	}

	auto emptyTiles = (worldSize_v.x * worldSize_v.y) - (float)occupied.size();
	if (emptyTiles <= 0) {
		return;
	}

	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> dist(0, (int)emptyTiles - 1);
	auto target = dist(rng);

	auto count = 0;
	for (float y = 0; y < worldSize_v.y; ++y) {
		for (float x = 0; x < worldSize_v.x; ++x) {
			auto id = static_cast<int>((y * worldSize_v.x) + x);
			if (!occupied.contains(id)) {
				if (count == target) {
					m_collectibles.emplace_back(*m_collectibleTexture, worldSpace::gridToWorld({x, y}));
					return;
				}
				count++;
			}
		}
	}
}

void Game::collideCollectibles() {
	for (auto it = m_collectibles.begin(); it != m_collectibles.end();) {
		if (it->getGridPosition() == worldSpace::worldToGrid(m_player->getSegments().back().transform.position)) {

			m_collectibles.erase(it); // erase collided collectible
			m_player->grow();		  // tell the player to not pop the tail/grow
			spawnCollectible();		  // spawn new collectible

			return;
		}
		it++;
	}
}

void Game::onGoBack() {
	m_log.debug("execute 'go back' action here");
}
} // namespace chomper::runtime
