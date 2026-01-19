#pragma once
#include "chomper/collectible.hpp"
#include "chomper/engine.hpp"
#include "chomper/player.hpp"
#include "chomper/runtime.hpp"
#include "chomper/world.hpp"
#include "le2d/random.hpp"
#include "le2d/resource/texture.hpp"
#include <klib/ptr.hpp>
#include <le2d/drawable/text.hpp>
#include <le2d/input/action.hpp>
#include <le2d/input/scoped_mapping.hpp>
#include <unordered_set>

namespace chomper::runtime {
// driven by Engine, owner (whether indirectly) of all game things.
class Game : public IRuntime, public klib::Pinned {
  public:
	explicit Game(gsl::not_null<Engine*> engine);

  private:
	// all Game-level input actions.
	struct Actions {
		le::input::action::KeyDigital goBackKey{GLFW_KEY_ESCAPE};
	};

	// IRuntime
	void tick(kvf::Seconds dt) final;
	void render(le::IRenderer& renderer) const final;

	void debugInspectWindow();

	void bindActions();
	void createPlayer();
	void createCollectibleTexture();

	void findEmptyTiles();
	void spawnCollectibles();
	void collideCollectibles();

	void onGoBack();

	klib::TypedLogger<Game> m_log{};

	gsl::not_null<Engine*> m_engine;

	le::input::ScopedActionMapping m_mapping;
	Actions m_actions{};

	le::Random m_random{};
	std::unordered_set<int> m_occupied;

	std::unique_ptr<Player> m_player{};
	std::unique_ptr<World> m_world{};
	std::vector<Collectible> m_collectibles{};
	klib::Ptr<le::ITexture const> m_collectibleTexture{};

	std::vector<int> m_emptyTiles{};

	le::drawable::Text m_countdownText{};
	kvf::Seconds m_countdown{3};
};
} // namespace chomper::runtime
