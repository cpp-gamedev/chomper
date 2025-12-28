#pragma once
#include "chomper/controller.hpp"
#include "chomper/engine.hpp"
#include "chomper/runtime.hpp"
#include <klib/ptr.hpp>
#include <le2d/input/action.hpp>
#include <le2d/input/scoped_mapping.hpp>

namespace chomper {
// driven by Engine, owner (whether indirectly) of all game things.
class Game : public IRuntime, public IController::IListener, public klib::Pinned {
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

	// IController::IListener
	void onSetHeading(Heading heading) final;

	void createController();
	void bindActions();

	void onGoBack();

	klib::TypedLogger<Game> m_log{};

	gsl::not_null<Engine*> m_engine;

	le::input::ScopedActionMapping m_mapping;
	Actions m_actions{};
	std::unique_ptr<IController> m_controller{};

	Heading m_heading{};
};
} // namespace chomper
