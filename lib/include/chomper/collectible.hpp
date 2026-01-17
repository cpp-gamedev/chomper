#pragma once
#include "chomper/world_space.hpp"
#include "glm/ext/vector_float2.hpp"
#include "le2d/drawable/sprite.hpp"
#include "le2d/renderer.hpp"
#include "le2d/resource/texture.hpp"

namespace chomper {
class Collectible {
  public:
	explicit Collectible(le::ITexture const& texture, glm::vec2 position);

	void draw(le::IRenderer& renderer) const;

	[[nodiscard]] glm::vec2 getGridPosition() const {
		return worldSpace::worldToGrid(m_sprite.transform.position);
	}

  private:
	le::drawable::Sprite m_sprite{};
};
} // namespace chomper