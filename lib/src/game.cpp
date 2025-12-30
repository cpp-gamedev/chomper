#include "chomper/game.hpp"
#include "chomper/im_util.hpp"
#include <imgui.h>
#include <klib/fixed_string.hpp>
#include <array>

namespace chomper {
namespace {
class StatsInspector : public IDebugInspector {
  public:
	explicit StatsInspector(DebugStats const& stats) : m_stats(stats) {}

  private:
	void debugInspect() final {
		auto const dt = std::chrono::duration<float, std::milli>{m_stats.frame.total_dt};
		ImGui::TextUnformatted(klib::FixedString{"frametime : {:.1f}ms", dt.count()}.c_str());
		ImGui::TextUnformatted(klib::FixedString{"framerate : {}", m_stats.frame.framerate}.c_str());
		ImGui::TextUnformatted(klib::FixedString{"frames : {}", m_stats.frame.total_frames}.c_str());
		ImGui::TextUnformatted(klib::FixedString{"runtime : {:.1f}s", m_stats.frame.run_time.count()}.c_str());
		ImGui::TextUnformatted(klib::FixedString{"draw calls : {}", m_stats.render.draw_calls}.c_str());
		ImGui::TextUnformatted(klib::FixedString{"triangles : {}", m_stats.render.triangles}.c_str());
	}

	DebugStats const& m_stats;
};
} // namespace

using ActionValue = le::input::action::Value;

Game::Game(gsl::not_null<Engine*> engine) : m_engine(engine), m_mapping(&engine->getInputRouter()) {
	createPlayer();
	m_world = std::make_unique<World>(m_engine);
}

void Game::tick(kvf::Seconds const dt) {
	m_player->tick(dt);

	ImGui::SetNextWindowSize({300.0f, 200.0f}, ImGuiCond_Once);
	if (ImGui::Begin("Debug Inspect")) {
		debugInspectWindow();
	}
	ImGui::End();
}

void Game::render(le::IRenderer& renderer) const {
	m_world->render(renderer);
	m_player->render(renderer);
}

void Game::debugInspectWindow() {
	auto statsInspector = StatsInspector{m_engine->getDebugStats()};

	auto const inspectItems = std::array{
		InspectItem{.inspector = m_player.get(), .label = "Player"},
		InspectItem{.inspector = &m_engine->getResources(), .label = "Resources"},
		InspectItem{.inspector = &statsInspector, .label = "Stats"},
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
	m_player = std::make_unique<Player>(m_mapping);
}

void Game::onGoBack() {
	m_log.debug("execute 'go back' action here");
}
} // namespace chomper
