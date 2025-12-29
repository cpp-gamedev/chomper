#include "chomper/resources.hpp"

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
} // namespace chomper
