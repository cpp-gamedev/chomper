#pragma once
#include <klib/base_types.hpp>
#include <kvf/time.hpp>
#include <le2d/renderer.hpp>

namespace chomper {
// driven by Engine.
class IRuntime : public klib::Polymorphic {
  public:
	// called at the start of every frame, update game state here.
	virtual void tick(kvf::Seconds dt) = 0;
	// called at the end of every frame, render game state here (mutation is prohibited).
	virtual void render(le::IRenderer& renderer) const = 0;
};
} // namespace chomper
