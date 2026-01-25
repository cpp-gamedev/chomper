#include "chomper/manifest/manifest_loader.hpp"
#include <klib/assert.hpp>
#include <le2d/resource/audio_buffer.hpp>
#include <le2d/resource/font.hpp>
#include <le2d/resource/texture.hpp>

namespace chomper {
ManifestLoader::ManifestLoader(le::AssetLoader asset_loader) : m_asset_loader(std::move(asset_loader)) {}

void ManifestLoader::startLoad(AssetManifest const& assetManifest) {
	// call startLoads<AssetType>() for each field in the manifest.
	startLoads<le::ITexture>(assetManifest.textures);
	startLoads<le::IAudioBuffer>(assetManifest.audio);
	startLoads<le::IFont>(assetManifest.fonts);
}

auto ManifestLoader::getProgress() const -> Progress {
	return Progress{
		.remaining = std::int64_t(m_loading.size()),
		.loaded = std::int64_t(m_loaded.size()),
	};
}

auto ManifestLoader::update() -> Progress {
	auto const isReady = [this](std::future<Asset>& future) {
		// should only have futures from std::async().
		KLIB_ASSERT(future.valid());

		if (future.wait_for(0s) == std::future_status::ready) { // if future is ready,
			m_loaded.push_back(future.get());					// store loaded asset, and
			return true;										// erase corresponding future.
		}
		return false; // else do nothing.
	};
	std::erase_if(m_loading, isReady);
	return getProgress();
}

void ManifestLoader::transferTo(Resources& out) {
	// block until all pending loads have been completed.
	for (auto& future : m_loading) {
		m_loaded.push_back(future.get());
	}
	m_loading.clear();

	// transfer valid loaded assets to out.
	auto count = std::int64_t{};
	for (auto& asset : m_loaded) {
		if (!asset.asset) {
			continue;
		}
		out.store(std::move(asset.uri), std::move(asset.asset));
		++count;
	}
	m_loaded.clear();

	m_log.info("{} asset(s) transferred to {}", count, klib::demangled_name(out));
}

template <std::derived_from<le::IAsset> AssetTypeT>
void ManifestLoader::startLoads(std::span<std::string const> uris) {
	for (auto const& uri : uris) {
		if (uri.empty()) {
			continue;
		}
		auto const loadAsset = [this, uri] {
			return Asset{.uri = std::move(uri), .asset = m_asset_loader.load<AssetTypeT>(uri)};
		};
		m_loading.push_back(std::async(loadAsset));
	}
}
} // namespace chomper
