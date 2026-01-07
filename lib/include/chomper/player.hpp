#pragma once
#include "chomper/controller.hpp"
#include "chomper/debug_inspector.hpp"
#include "chomper/snake.hpp"
#include <imgui.h>
#include <klib/log.hpp>
#include <le2d/input/action.hpp>
#include <le2d/input/scoped_mapping.hpp>
#include <le2d/renderer.hpp>

namespace chomper {
class Player : public IController::IListener, public IDebugInspector, public klib::Pinned {
  public:
	explicit Player(le::input::ScopedActionMapping& mapping);

	void tick(kvf::Seconds dt);
	void draw(le::IRenderer& renderer) const;

  private:
	// IController::IListener
	void onSetHeading(Heading heading) final;

	// IDebugInspector
	void debugInspect() final;

	void createController(le::input::ScopedActionMapping& mapping);

	klib::TypedLogger<Player> m_log{};

	std::unique_ptr<IController> m_controller{};

	Snake m_snake{};
};
} // namespace chomper
