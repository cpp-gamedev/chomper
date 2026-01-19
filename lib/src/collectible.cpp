#include "chomper/collectible.hpp"
#include "chomper/world_size.hpp"
#include <le2d/renderer.hpp>

namespace chomper {
Collectible::Collectible(le::ITexture const& texture, glm::vec2 position) {
	m_sprite.set_base_size(tileSize_v);
	m_sprite.set_texture(&texture);
	m_sprite.transform.position = position;
}

void Collectible::draw(le::IRenderer& renderer) const {
	m_sprite.draw(renderer);
}
} // namespace chomper
