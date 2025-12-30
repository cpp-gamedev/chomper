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
} // namespace chomper
