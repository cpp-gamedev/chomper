#pragma once
#include "chomper/engine.hpp"
#include "chomper/runtime.hpp"
#include <le2d/drawable/text.hpp>

namespace chomper::runtime {
class MainMenu : public IRuntime {
  public:
	explicit MainMenu(gsl::not_null<Engine*> engine);

  private:
	void tick(kvf::Seconds dt) final;
	void render(le::IRenderer& renderer) const final;

	void swingMainText(kvf::Seconds dt);

	gsl::not_null<Engine*> m_engine;

	le::drawable::Text m_mainText{};
	kvf::Seconds m_elapsed{};
};
} // namespace chomper::runtime
