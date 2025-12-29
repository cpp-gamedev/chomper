#pragma once
#include "chomper/viewport.hpp"
#include <kvf/bitmap.hpp>
#include <le2d/context.hpp>
#include <le2d/drawable/shape.hpp>
#include <le2d/renderer.hpp>
#include <le2d/resource/texture.hpp>

namespace chomper {
class World {
  public:
	explicit World(le::Context const& context);

	void render(le::IRenderer& renderer) const;

  private:
	void createGrid(le::Context const& context);

	le::drawable::Quad m_gridQuad;
	std::unique_ptr<le::ITexture> m_gridTexture;
};

} // namespace chomper
