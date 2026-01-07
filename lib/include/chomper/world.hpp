#pragma once
#include "chomper/viewport.hpp"
#include <kvf/bitmap.hpp>
#include <le2d/context.hpp>
#include <le2d/drawable/shape.hpp>
#include <le2d/renderer.hpp>
#include <le2d/resource/texture.hpp>

namespace chomper {
class Engine;
class World {
  public:
	explicit World(gsl::not_null<Engine const*> engine);

	void draw(le::IRenderer& renderer) const;

  private:
	void createGrid();

	gsl::not_null<Engine const*> m_engine;

	le::drawable::Quad m_gridQuad;
	std::unique_ptr<le::ITexture> m_gridTexture;
};

} // namespace chomper
