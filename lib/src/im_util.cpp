#include "chomper/im_util.hpp"
#include <imgui.h>

namespace chomper {
void im_util::inspectAsTabs(std::span<InspectItem const> items) {
	if (ImGui::BeginTabBar("tabs")) {
		for (auto const& item : items) {
			if (ImGui::BeginTabItem(item.label.c_str())) {
				item.inspectable->debugInspect();
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

void im_util::textColored(kvf::Color const color, klib::CString const text) {
	auto const gamma_corrected = color.to_linear();
	auto const im_color = ImVec4{gamma_corrected.x, gamma_corrected.y, gamma_corrected.z, gamma_corrected.w};
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
	ImGui::TextColored(im_color, "%s", text.c_str());
}
} // namespace chomper
