#include "chomper/world.hpp"
#include "chomper/world_size.hpp"

namespace chomper {
World::World(le::Context const& context) {
	createGrid(context);
}

void World::render(le::IRenderer& renderer) const {
	m_gridQuad.draw(renderer);
}

void World::createGrid(le::Context const& context) {
	m_gridQuad.create(viewport_v.world_size);

	// Make the bitmap
	auto bitmap = kvf::ColorBitmap{glm::vec2{2}};
	bitmap[0, 0] = kvf::Color{0xAAD751FF};
	bitmap[0, 1] = kvf::Color{0xA2D149FF};
	bitmap[1, 1] = kvf::Color{0xAAD751FF};
	bitmap[1, 0] = kvf::Color{0xA2D149FF};

	// Create / apply the texture
	m_gridTexture = context.get_resource_factory().create_texture({.wrap = vk::SamplerAddressMode::eRepeat, .filter = vk::Filter::eNearest});
	m_gridTexture->overwrite(bitmap.bitmap());

	m_gridQuad.texture = m_gridTexture.get();
	m_gridQuad.create(m_gridQuad.get_rect(), kvf::UvRect{.lt = {0.f, 0.f}, .rb = world_size_v});
}

} // namespace chomper
