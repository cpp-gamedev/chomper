#pragma once
#include <djson/string_table.hpp>
#include <le2d/asset/asset.hpp>
#include <le2d/asset/asset_loader.hpp>
#include <le2d/resource/font.hpp>
#include <format>
#include <memory>
#include <stdexcept>

namespace chomper {
class Resources {
  public:
	explicit Resources(le::AssetLoader assetLoader);

	template <std::derived_from<le::IAsset> AssetTypeT>
	[[nodiscard]] AssetTypeT* load(std::string_view const uri) {
		auto it = m_assets.find(uri);
		if (it == m_assets.end()) {
			auto ret = m_assetLoader.load<AssetTypeT>(uri);
			if (!ret) {
				return {};
			}
			it = m_assets.insert_or_assign(std::string{uri}, std::move(ret)).first;
		}
		KLIB_ASSERT(it != m_assets.end());
		return dynamic_cast<AssetTypeT*>(it->second.get());
	}

	template <std::derived_from<le::IAsset> AssetTypeT>
	[[nodiscard]] AssetTypeT& loadRequired(std::string_view const uri) {
		if (auto* ret = load<AssetTypeT>(uri)) {
			return *ret;
		}
		throw std::runtime_error{std::format("Failed to load required {}: {}", klib::demangled_name<AssetTypeT>(), uri)};
	}

	[[nodiscard]] le::IFont& getMainFont() const {
		return *m_mainFont;
	}

	bool unload(std::string_view uri);
	std::size_t unloadAll();

	[[nodiscard]] std::size_t assetCount() const {
		return m_assets.size();
	}

	[[nodiscard]] bool isRequiredAsset(le::IAsset const& asset) const;

  private:
	le::AssetLoader m_assetLoader{};
	dj::StringTable<std::unique_ptr<le::IAsset>> m_assets{};

	gsl::not_null<le::IFont*> m_mainFont;
};
} // namespace chomper
