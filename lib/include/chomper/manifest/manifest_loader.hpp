#pragma once
#include "chomper/manifest/asset_manifest.hpp"
#include "chomper/resources.hpp"
#include <klib/log.hpp>
#include <le2d/asset/asset.hpp>
#include <le2d/asset/asset_loader.hpp>
#include <future>
#include <memory>
#include <span>

namespace chomper {
class ManifestLoader {
  public:
	struct Progress;

	explicit ManifestLoader(le::AssetLoader asset_loader);

	void startLoad(AssetManifest const& manifest);

	Progress update();
	Progress getProgress() const;

	void transferTo(Resources& out);

  private:
	struct Asset {
		std::string uri{};
		std::unique_ptr<le::IAsset> asset{};
	};

	template <std::derived_from<le::IAsset> AssetTypeT>
	void startLoads(std::span<std::string const> uris);

	klib::TypedLogger<ManifestLoader> m_log{};

	le::AssetLoader m_asset_loader{};

	std::vector<std::future<Asset>> m_loading{}; // assets being loaded.
	std::vector<Asset> m_loaded{};				 // loaded assets.
};

struct ManifestLoader::Progress {
	[[nodiscard]] constexpr std::int64_t getTotal() const {
		return remaining + loaded;
	}

	[[nodiscard]] constexpr float normalized() const {
		auto const total = getTotal();
		if (total == 0) {
			return 0.0f;
		}
		return float(loaded) / float(total);
	}

	[[nodiscard]] constexpr bool isLoading() const {
		return remaining > 0;
	}

	std::int64_t remaining{};
	std::int64_t loaded{};
};
} // namespace chomper
