#include "chomper/engine.hpp"
#include "chomper/build_version.hpp"
#include "chomper/game.hpp"
#include "chomper/inclusive_range.hpp"
#include "chomper/viewport.hpp"
#include <imgui.h>
#include <klib/assert.hpp>
#include <klib/fixed_string.hpp>
#include <klib/visitor.hpp>
#include <le2d/file_data_loader.hpp>
#include <le2d/util.hpp>

namespace chomper {
namespace {
constexpr auto clearColor_v = kvf::Color{glm::vec4{.34f, .54f, .2f, 1.f}};
}
Engine::Engine(CreateInfo const& createInfo) : m_prefs(createInfo.prefsPath) {
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
		auto const realDt = deltaTime.tick();
		auto scaledDt = m_dtScale * realDt;
		static constexpr auto maxDt_v = kvf::Seconds{2.0f};
		if (scaledDt > maxDt_v) {
			m_log.warn("giant dt ({}), clamping to max ({})", scaledDt, maxDt_v);
			scaledDt = maxDt_v;
		}
		processEvents();
		m_inputRouter.dispatch(m_context->event_queue());
		m_runtime->tick(scaledDt);

		// render runtime.
		auto& renderer = m_context->begin_render(clearColor_v);
		renderer.viewport = viewport_v;
		renderer.polygon_mode = m_wireframe ? vk::PolygonMode::eLine : vk::PolygonMode::eFill;

		m_runtime->render(renderer);
		renderer.end_render();

		m_debugStats = DebugStats{
			.frame = m_context->get_frame_stats(),
			.render = renderer.get_stats(),
		};

		// submit frame for presentation.
		m_context->present();
	}
}

void Engine::setVsync(le::Vsync const vsync) {
	m_context->set_vsync(vsync);
	m_prefs.setVsync(m_context->get_vsync());
}

void Engine::debugInspect() {
	inspectStats();
	ImGui::Separator();
	inspectVsync();
	inspectDtScale();
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
	static constexpr auto windowSize_v = glm::ivec2{800, 800};
	static constexpr auto windowFlags_v = le::default_window_flags_v & ~le::WindowFlag::Visible;

	auto const windowSize = m_prefs.getWindowSize().value_or(windowSize_v);
	auto const windowInfo = le::WindowInfo{
		.size = windowSize,
		.title = windowTitle.c_str(),
		.flags = windowFlags_v,
	};
	auto const contextCI = le::Context::CreateInfo{
		.platform_flags = platformFlags,
		.window = windowInfo,
	};
	m_context = le::Context::create(contextCI);

	if (auto const vsync = m_prefs.getVsync()) {
		setVsync(*vsync);
	}
}

void Engine::createResources() {
	auto assetLoader = m_context->create_asset_loader(m_dataLoader.get());
	m_resources = std::make_unique<Resources>(std::move(assetLoader));
}

void Engine::createRuntime() {
	// Game stores 'this', so Engine must remain address-stable. this is why it inherits from Pinned.
	m_runtime = std::make_unique<Game>(this);
}

void Engine::inspectStats() {
	ImGui::TextUnformatted("stats");
	auto const dt = std::chrono::duration<float, std::milli>{m_debugStats.frame.total_dt};
	ImGui::TextUnformatted(klib::FixedString{"frametime : {:.1f}ms", dt.count()}.c_str());
	ImGui::TextUnformatted(klib::FixedString{"framerate : {}", m_debugStats.frame.framerate}.c_str());
	ImGui::TextUnformatted(klib::FixedString{"frames : {}", m_debugStats.frame.total_frames}.c_str());
	ImGui::TextUnformatted(klib::FixedString{"runtime : {:.1f}s", m_debugStats.frame.run_time.count()}.c_str());
	ImGui::TextUnformatted(klib::FixedString{"draw calls : {}", m_debugStats.render.draw_calls}.c_str());
	ImGui::TextUnformatted(klib::FixedString{"triangles : {}", m_debugStats.render.triangles}.c_str());
}

void Engine::inspectVsync() {
	auto const supported = m_context->get_supported_vsync();
	auto const current = m_context->get_vsync();
	auto selected = std::optional<le::Vsync>{};
	if (ImGui::BeginCombo("vsync", le::vsync_str_v[current].data())) {
		for (auto const vsync : supported) {
			auto const str = le::vsync_str_v[vsync];
			if (ImGui::Selectable(str.data(), vsync == current)) {
				selected = vsync;
			}
		}
		ImGui::EndCombo();
	}

	if (selected) {
		setVsync(*selected);
	}
}

void Engine::inspectDtScale() {
	static constexpr auto range_v = InclusiveRange{.lo = 0.0f, .hi = 3.0f};
	auto dtScale = m_dtScale;
	if (ImGui::DragFloat("dt scale", &dtScale, 0.05f, range_v.lo, range_v.hi)) {
		m_dtScale = std::clamp(dtScale, range_v.lo, range_v.hi);
	}
	KLIB_ASSERT(m_dtScale >= 0.0f);

	auto paused = m_dtScale == 0.0f;
	if (ImGui::Checkbox("pause", &paused)) {
		m_dtScale = paused ? 0.0f : 1.0f;
	}
}

void Engine::processEvents() {
	auto const visitor = klib::SubVisitor{
		[this](le::event::WindowResize const& e) {
			m_prefs.setWindowSize(e);
		},
	};
	for (auto const& event : m_context->event_queue()) {
		std::visit(visitor, event);
	}
}
} // namespace chomper
