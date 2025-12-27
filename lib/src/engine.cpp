#include "chomper/engine.hpp"
#include "chomper/build_version.hpp"
#include <imgui.h>
#include <le2d/file_data_loader.hpp>

namespace chomper {
Engine::Engine(CreateInfo create_info) {
	m_data_loader = std::make_unique<le::FileDataLoader>(create_info.assets_dir);

	auto const platform_flags = [&] {
		auto ret = le::PlatformFlag::None;
		if (create_info.no_libdecor) { ret |= le::PlatformFlag::NoLibdecor; }
		return ret;
	}();
	auto const window_title = std::format("chomper {}", build_version_str);
	auto const context_ci = le::Context::CreateInfo{
		.platform_flags = platform_flags,
		.window = le::WindowInfo{.size = {1280, 720}, .title = window_title.c_str()},
	};
	m_context = le::Context::create(context_ci);
	m_log.info("Engine created");
}

void Engine::run() {
	while (m_context->is_running()) {
		m_context->next_frame();
		auto& renderer = m_context->begin_render();
		// TODO
		renderer.end_render();
		ImGui::ShowDemoWindow();
		m_context->present();
	}
}
} // namespace chomper
