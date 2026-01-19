#include "chomper/runtimes/game.hpp"
#include "chomper/collectible.hpp"
#include "chomper/im_util.hpp"
#include "chomper/runtimes/entrypoint.hpp"
#include "chomper/world_size.hpp"
#include "chomper/world_space.hpp"
#include <le2d/random.hpp>
#include <algorithm>

namespace chomper::runtime {
namespace {
constexpr auto countdownParams_v = le::drawable::Text::Params{
	.height = le::TextHeight{60},
};
constexpr auto collectibleAmount_v = 10;
} // namespace
using ActionValue = le::input::action::Value;

Game::Game(gsl::not_null<Engine*> engine) : m_engine(engine), m_mapping(&engine->getInputRouter()) {
	createPlayer();
	m_world = std::make_unique<World>(m_engine);

	createCollectibleTexture();

	spawnCollectibles();

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

	collideCollectibles();

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

void Game::createCollectibleTexture() {
	m_collectibleTexture = m_engine->getResources().load<le::ITexture>("images/apple.png");
}

void Game::findEmptyTiles() {
	m_emptyTiles.clear();
	m_emptyTiles.reserve(static_cast<int>(worldSize_v.x * worldSize_v.y));
	for (auto i = 0; i < static_cast<int>(worldSize_v.x * worldSize_v.y); i++) {
		m_emptyTiles.push_back(i);
	}

	auto const removeTile = [this](int tile) {
		auto it = std::ranges::find(m_emptyTiles, tile);
		if (it != m_emptyTiles.end()) {
			*it = m_emptyTiles.back();
			m_emptyTiles.pop_back();
		}
	};

	for (auto const& seg : m_player->getSegments()) {
		auto p = worldSpace::worldToGrid(seg.transform.position);
		removeTile(static_cast<int>((p.y * worldSize_v.x) + p.x));
	}

	for (auto const& c : m_collectibles) {
		auto p = c.getGridPosition();
		removeTile(static_cast<int>((p.y * worldSize_v.x) + p.x));
	}
}

void Game::spawnCollectibles() {
	findEmptyTiles();

	for (auto i = m_collectibles.size(); i < collectibleAmount_v; i++) {
		if (m_emptyTiles.empty()) {
			return;
		}
		// find a random tile
		auto random = m_random.next_index(m_emptyTiles.size());
		auto tile = m_emptyTiles[random];
		// remove said tile from the vector
		std::erase_if(m_emptyTiles, [&](auto const& v) {
			return v == m_emptyTiles[random];
		});
		// place the collectible on the tile
		auto width = static_cast<int>(worldSize_v.x);
		m_collectibles.emplace_back(*m_collectibleTexture, worldSpace::gridToWorld({tile % width, tile / width}));
	}
}

void Game::collideCollectibles() {
	auto it = std::ranges::find_if(m_collectibles, [&](auto const& collectible) {
		return collectible.getGridPosition() == worldSpace::worldToGrid(m_player->getSegments().back().transform.position);
	});
	if (it == m_collectibles.end()) {
		return;
	}

	m_collectibles.erase(it);
	m_player->grow();
	spawnCollectibles();
}

void Game::onGoBack() {
	m_log.debug("execute 'go back' action here");
}
} // namespace chomper::runtime
