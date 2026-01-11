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
class Engine;

class Player : public IController::IListener, public IDebugInspector, public klib::Pinned {
  public:
	explicit Player(le::input::ScopedActionMapping& mapping, gsl::not_null<Engine*> engine);

	void tick(kvf::Seconds dt);
	void draw(le::IRenderer& renderer) const;

  private:
	bool selfCollides() const;
	void move();

	// IController::IListener
	void onSetHeading(Heading heading) final;

	// IDebugInspector
	void debugInspect() final;

	void createController(le::input::ScopedActionMapping& mapping);

	klib::TypedLogger<Player> m_log{};

	gsl::not_null<Engine*> m_engine;

	std::unique_ptr<IController> m_controller{};

	Snake m_snake{};

	Heading m_heading{};
	std::vector<Heading> m_headingQueue{};

	kvf::Seconds m_moveTimer{};
	// bool to decide wether to remove the tail, turn false if the snake has eaten
	bool m_shouldPop = true;
	bool m_graceMove{};
};
} // namespace chomper
