#include "chomper/im_util.hpp"
#include <imgui.h>

namespace chomper {
void im_util::inspectAsTabs(std::span<InspectItem const> items) {
	if (ImGui::BeginTabBar("tabs")) {
		for (auto const& item : items) {
			if (ImGui::BeginTabItem(item.label.c_str())) {
				item.inspector->debugInspect();
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

void im_util::textColored(kvf::Color const color, klib::CString const text) {
	auto const gammaCorrected = color.to_linear();
	auto const imColor = ImVec4{gammaCorrected.x, gammaCorrected.y, gammaCorrected.z, gammaCorrected.w};
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
	ImGui::TextColored(imColor, "%s", text.c_str());
}
} // namespace chomper
