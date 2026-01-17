#include "chomper/collectible.hpp"
#include "chomper/world_size.hpp"
#include "le2d/renderer.hpp"

namespace chomper {
Collectible::Collectible(le::ITexture const& texture, glm::vec2 position) {
	m_quad.create(tileSize_v);
	m_quad.texture = &texture;
	m_quad.transform.position = position;
}

void Collectible::draw(le::IRenderer& renderer) const {
	m_quad.draw(renderer);
}
} // namespace chomper