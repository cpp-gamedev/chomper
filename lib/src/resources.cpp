#include "chomper/resources.hpp"
#include "chomper/im_util.hpp"
#include <imgui.h>
#include <klib/fixed_string.hpp>

namespace chomper {
Resources::Resources(le::AssetLoader assetLoader) : m_assetLoader(std::move(assetLoader)), m_mainFont(&loadRequired<le::IFont>("fonts/main.ttf")) {}

bool Resources::unload(std::string_view const uri) {
	auto const it = m_assets.find(uri);
	if (it == m_assets.end() || isRequiredAsset(*it->second)) {
		return false;
	}
	m_assets.erase(it);
	return true;
}

std::size_t Resources::unloadAll() {
	auto ret = 0uz;
	for (auto it = m_assets.begin(); it != m_assets.end();) {
		if (isRequiredAsset(*it->second)) {
			++it;
			continue;
		}

		it = m_assets.erase(it);
		++ret;
	}
	return ret;
}

bool Resources::isRequiredAsset(le::IAsset const& asset) const {
	return &asset == m_mainFont;
}

void Resources::debugInspect() {
	ImGui::TextUnformatted(klib::FixedString{"asset count: {}", assetCount()}.c_str());
	ImGui::Separator();

	static constexpr auto required_color_v = kvf::Color{0xffaa00ff};
	for (auto const& [uri, asset] : m_assets) {
		auto const text = klib::FixedString<128>{"[{}] {}", klib::demangled_name(*asset), uri};
		if (isRequiredAsset(*asset)) {
			im_util::textColored(required_color_v, text.c_str());
		} else {
			ImGui::TextUnformatted(text.c_str());
		}
	}
}
} // namespace chomper
