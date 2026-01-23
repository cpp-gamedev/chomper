#include "chomper/runtimes/entrypoint.hpp"
#include "chomper/runtimes/main_menu.hpp"
#include <klib/log.hpp>
#include <klib/visitor.hpp>

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
	m_manifestLoader.startLoad(assetManifest);
}

void Entrypoint::tick(kvf::Seconds const /*dt*/) {
	auto const loadProgress = m_manifestLoader.update();

	if (loadProgress.isLoading()) {
		return;
	}

	m_manifestLoader.transferTo(m_engine->getResources());
	m_engine->setNextRuntime<MainMenu>();
}

void Entrypoint::render(le::IRenderer& renderer) const {
	m_mainText.draw(renderer);
}

void Entrypoint::setupMainText() {
	static constexpr auto textParams_v = le::drawable::Text::Params{
		.height = le::TextHeight{60},
	};
	m_mainText.set_string(m_engine->getResources().getMainFont(), "LOADING...", textParams_v);
}
} // namespace chomper::runtime
