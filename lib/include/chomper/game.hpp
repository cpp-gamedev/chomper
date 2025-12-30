#pragma once
#include "chomper/engine.hpp"
#include "chomper/player.hpp"
#include "chomper/runtime.hpp"
#include <klib/ptr.hpp>
#include <le2d/input/action.hpp>
#include <le2d/input/scoped_mapping.hpp>

namespace chomper {
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

	void onGoBack();

	klib::TypedLogger<Game> m_log{};

	gsl::not_null<Engine*> m_engine;

	le::input::ScopedActionMapping m_mapping;
	Actions m_actions{};

	std::unique_ptr<Player> m_player{};
};
} // namespace chomper
