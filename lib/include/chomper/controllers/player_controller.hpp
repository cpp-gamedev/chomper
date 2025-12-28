#pragma once
#include "chomper/controller.hpp"
#include <klib/enum_array.hpp>
#include <le2d/input/action.hpp>
#include <le2d/input/action_mapping.hpp>
#include <vector>

namespace chomper {
// player driven Heading controller.
class PlayerController : public IController {
  public:
	explicit PlayerController(gsl::not_null<IListener*> listener, le::input::ActionMapping& mapping);

  private:
	// list(s) of input actions (mapped to a single Heading).
	using ActionList = std::vector<std::unique_ptr<le::input::IAction>>;

	using TurnActions = klib::EnumArray<Heading, ActionList>;

	void tick(kvf::Seconds dt) final;

	void create_turn_actions();
	void bind_actions(le::input::ActionMapping& mapping);

	TurnActions m_turn_actions{};
};
} // namespace chomper
