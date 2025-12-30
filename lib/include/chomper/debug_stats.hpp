#pragma once
#include <le2d/frame_stats.hpp>
#include <le2d/render_stats.hpp>

namespace chomper {
struct DebugStats {
	le::FrameStats frame{};
	le::RenderStats render{};
};
} // namespace chomper
