#include "chomper/engine.hpp"
#include "chomper/build_version.hpp"
#include <imgui.h>
#include <le2d/file_data_loader.hpp>

namespace chomper {
Engine::Engine(CreateInfo const& create_info) {
	create_data_loader(create_info.assets_dir);
	create_context(create_info);

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

void Engine::create_data_loader(std::string_view const assets_dir) { m_data_loader = std::make_unique<le::FileDataLoader>(assets_dir); }

void Engine::create_context(CreateInfo const& create_info) {
	auto const platform_flags = [&] {
		auto ret = le::PlatformFlag::None;
		if (create_info.no_libdecor) { ret |= le::PlatformFlag::NoLibdecor; }
		return ret;
	}();
	auto const window_title = std::format("chomper {}", build_version_str);
	static constexpr auto window_size_v = glm::ivec2{800, 800};
	static constexpr auto window_flags_v = le::default_window_flags_v & ~le::WindowFlag::Visible;
	auto const window_info = le::WindowInfo{
		.size = window_size_v,
		.title = window_title.c_str(),
		.flags = window_flags_v,
	};
	auto const context_ci = le::Context::CreateInfo{
		.platform_flags = platform_flags,
		.window = window_info,
	};
	m_context = le::Context::create(context_ci);
	m_context->lock_aspect_ratio(true);
	m_context->set_visible(true);
}
} // namespace chomper
