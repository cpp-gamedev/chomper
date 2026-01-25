#pragma once
#include "chomper/debug_inspector.hpp"
#include <djson/string_table.hpp>
#include <klib/log.hpp>
#include <le2d/asset/asset.hpp>
#include <le2d/asset/asset_loader.hpp>
#include <le2d/resource/font.hpp>
#include <format>
#include <memory>
#include <stdexcept>

namespace chomper {
class Resources : public IDebugInspector {
  public:
	explicit Resources(le::AssetLoader assetLoader);

	void store(std::string uri, std::unique_ptr<le::IAsset> asset);

	template <std::derived_from<le::IAsset> AssetTypeT>
	[[nodiscard]] AssetTypeT* load(std::string_view const uri) {
		auto it = m_assets.find(uri);
		if (it != m_assets.end()) {
			return dynamic_cast<AssetTypeT*>(it->second.get());
		}
		m_log.warn("loading {} at runtime: {}", klib::demangled_name<AssetTypeT>(), uri);
		return reload<AssetTypeT>(uri);
	}

	template <std::derived_from<le::IAsset> AssetTypeT>
	[[nodiscard]] AssetTypeT& loadRequired(std::string_view const uri) {
		if (auto* ret = load<AssetTypeT>(uri)) {
			return *ret;
		}
		throw std::runtime_error{std::format("Failed to load required {}: {}", klib::demangled_name<AssetTypeT>(), uri)};
	}

	template <std::derived_from<le::IAsset> AssetTypeT>
	[[nodiscard]] AssetTypeT* reload(std::string_view const uri) {
		auto asset = m_assetLoader.load<AssetTypeT>(uri);
		if (!asset) {
			m_log.warn("failed to load {}: {}", klib::demangled_name<AssetTypeT>(), uri);
			return {};
		}
		auto* ret = asset.get();
		m_assets.insert_or_assign(std::string{uri}, std::move(asset));
		return ret;
	}

	template <std::derived_from<le::IAsset> AssetTypeT>
	[[nodiscard]] AssetTypeT& reloadRequired(std::string_view const uri) {
		if (auto* ret = reload<AssetTypeT>(uri)) {
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
	// IDebugInspector
	void debugInspect() final;

	klib::TypedLogger<Resources> m_log{};

	le::AssetLoader m_assetLoader{};
	dj::StringTable<std::unique_ptr<le::IAsset>> m_assets{};

	gsl::not_null<le::IFont*> m_mainFont;
};
} // namespace chomper
