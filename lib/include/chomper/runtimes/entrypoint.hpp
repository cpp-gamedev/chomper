#pragma once
#include "chomper/engine.hpp"
#include "chomper/manifest/manifest_loader.hpp"
#include "chomper/runtime.hpp"
#include "chomper/ui/progress_bar.hpp"
#include <le2d/drawable/text.hpp>

namespace chomper::runtime {
class Entrypoint : public IRuntime {
  public:
	explicit Entrypoint(gsl::not_null<Engine*> engine);

  private:
	void tick(kvf::Seconds dt) final;
	void render(le::IRenderer& renderer) const final;

	void setupMainText();
	void setupProgressBar();

	gsl::not_null<Engine*> m_engine;

	ManifestLoader m_manifestLoader;

	le::drawable::Text m_mainText{};
	ui::ProgressBar m_progressBar{};
	float m_progress{};
};
} // namespace chomper::runtime
