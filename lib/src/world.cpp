#include "chomper/world.hpp"
#include "chomper/engine.hpp"
#include "chomper/world_size.hpp"

namespace chomper {
World::World(gsl::not_null<Engine const*> engine) : m_engine(engine) {
	createGrid();
}

void World::draw(le::IRenderer& renderer) const {
	m_gridQuad.draw(renderer);
}

void World::createGrid() {
	// Make the bitmap
	auto bitmap = kvf::ColorBitmap{glm::vec2{2}};
	bitmap[0, 0] = kvf::Color{0xAAD751FF};
	bitmap[0, 1] = kvf::Color{0xA2D149FF};
	bitmap[1, 1] = kvf::Color{0xAAD751FF};
	bitmap[1, 0] = kvf::Color{0xA2D149FF};

	// Create / apply the texture
	m_gridTexture = m_engine->getContext().get_resource_factory().create_texture({.wrap = vk::SamplerAddressMode::eRepeat, .filter = vk::Filter::eNearest});
	m_gridTexture->overwrite(bitmap.bitmap());

	m_gridQuad.texture = m_gridTexture.get();
	auto const rect = kvf::Rect<>::from_size(viewport_v.world_size);
	m_gridQuad.create(rect, kvf::UvRect{.rb = worldSize_v / 2}); // / 2, since each texture is 2x2 tiles
}

} // namespace chomper
