#include "chomper/engine.hpp"
#include "chomper/build_version.hpp"
#include "chomper/game.hpp"
#include "chomper/viewport.hpp"
#include <le2d/file_data_loader.hpp>
#include <le2d/util.hpp>

namespace chomper {
namespace {
constexpr auto clear_color_v = kvf::Color{glm::vec4{.34f, .54f, .2f, 1.f}};
}
Engine::Engine(CreateInfo const& createInfo) {
	createDataLoader(createInfo.assetsDir);
	createContext(createInfo);
	createResources();
	createRuntime();

	m_context->set_visible(true);
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
		auto& renderer = m_context->begin_render(clear_color_v);
		renderer.viewport = viewport_v;

		m_runtime->render(renderer);
		renderer.end_render();

		// submit frame for presentation.
		m_context->present();
	}
}

void Engine::createDataLoader(std::string_view assetsDir) {
	auto upfoundAssetsDir = std::string{};
	if (assetsDir.empty()) {
		upfoundAssetsDir = le::FileDataLoader::upfind("assets", le::util::exe_path());
		assetsDir = upfoundAssetsDir;
	}
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
}

void Engine::createResources() {
	auto assetLoader = m_context->create_asset_loader(m_dataLoader.get());
	m_resources = std::make_unique<Resources>(std::move(assetLoader));
}

void Engine::createRuntime() {
	// Game stores 'this', so Engine must remain address-stable. this is why it inherits from Pinned.
	m_runtime = std::make_unique<Game>(this);
}
} // namespace chomper
