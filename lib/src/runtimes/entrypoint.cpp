#include "chomper/runtimes/entrypoint.hpp"
#include "chomper/runtimes/main_menu.hpp"
#include "chomper/theme.hpp"
#include "chomper/viewport.hpp"
#include <klib/log.hpp>
#include <klib/visitor.hpp>
#include <algorithm>

namespace chomper::runtime {
namespace {
auto const assetManifest = AssetManifest{
	.textures =
		{
			"images/apple.png",
		},
};
} // namespace

Entrypoint::Entrypoint(gsl::not_null<Engine*> engine) : m_engine(engine), m_manifestLoader(engine->createAssetLoader()) {
	setupMainText();
	setupProgressBar();
	m_manifestLoader.startLoad(assetManifest);
}

void Entrypoint::tick(kvf::Seconds const dt) {
	auto const loadProgress = m_manifestLoader.update();
	auto const t = std::clamp(50.0f * dt.count(), 0.5f, 1.0f);
	m_progress = std::lerp(m_progress, loadProgress.normalized(), t);
	m_progressBar.setProgress(m_progress);

	static constexpr auto progressTrigger_v{0.95f};
	auto const aestheticWait = m_progress < progressTrigger_v; // functionally useless, looks good
	if (loadProgress.isLoading() || aestheticWait) {
		return;
	}

	m_manifestLoader.transferTo(m_engine->getResources());
	m_engine->setNextRuntime<MainMenu>();
}

void Entrypoint::render(le::IRenderer& renderer) const {
	m_progressBar.draw(renderer);
	m_mainText.draw(renderer);
}

void Entrypoint::setupMainText() {
	static constexpr auto textParams_v = le::drawable::Text::Params{
		.height = le::TextHeight{60},
	};
	m_mainText.set_string(m_engine->getResources().getMainFont(), "LOADING...", textParams_v);
}

void Entrypoint::setupProgressBar() {
	m_progressBar.targetSize = {0.8f * viewport_v.world_size.x, 50.0f};
	m_progressBar.quad.transform.position = {-0.5f * m_progressBar.targetSize.x, -200.0f};
	m_progressBar.quad.tint = theme::snakeBodyColor_v;
	m_progressBar.setProgress(0.0f);
}
} // namespace chomper::runtime
