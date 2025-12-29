#include "chomper/engine.hpp"
#include "chomper/build_version.hpp"
#include "chomper/game.hpp"
#include "chomper/viewport.hpp"
#include <le2d/file_data_loader.hpp>

namespace chomper {
Engine::Engine(CreateInfo const& createInfo) {
	createDataLoader(createInfo.assetsDir);
	createContext(createInfo);
	createRuntime();

	m_log.info("created");
}

void Engine::run() {
	auto deltaTime = kvf::DeltaTime{};
	m_log.info("starting game loop");
	while (m_context->is_running()) {
		// initialize next frame.
		m_context->next_frame();

		// dispatch events and tick runtime.
		auto const dt = deltaTime.tick();
		m_inputRouter.dispatch(m_context->event_queue());
		m_runtime->tick(dt);

		// render runtime.
		auto& renderer = m_context->begin_render();
		renderer.viewport = viewport_v;

		m_runtime->render(renderer);
		renderer.end_render();

		// submit frame for presentation.
		m_context->present();
	}
}

void Engine::createDataLoader(std::string_view const assetsDir) {
	m_dataLoader = std::make_unique<le::FileDataLoader>(assetsDir);
}

void Engine::createContext(CreateInfo const& createInfo) {
	auto const platformFlags = [&] {
		auto ret = le::PlatformFlag::None;
		if (createInfo.noLibdecor) {
			ret |= le::PlatformFlag::NoLibdecor;
		}
		return ret;
	}();
	auto const windowTitle = std::format("chomper {}", buildVersionStr);
	static constexpr auto window_size_v = glm::ivec2{800, 800};
	static constexpr auto window_flags_v = le::default_window_flags_v & ~le::WindowFlag::Visible;
	auto const windowInfo = le::WindowInfo{
		.size = window_size_v,
		.title = windowTitle.c_str(),
		.flags = window_flags_v,
	};
	auto const contextCI = le::Context::CreateInfo{
		.platform_flags = platformFlags,
		.window = windowInfo,
	};
	m_context = le::Context::create(contextCI);
	m_context->lock_aspect_ratio(true);
	m_context->set_visible(true);
}

void Engine::createRuntime() {
	// Game stores 'this', so Engine must remain address-stable. this is why it inherits from Pinned.
	m_runtime = std::make_unique<Game>(this);
}
} // namespace chomper
