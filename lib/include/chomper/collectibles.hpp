#pragma once
#include "chomper/player.hpp"
#include <le2d/drawable/sprite.hpp>
#include <le2d/random.hpp>
#include <le2d/renderer.hpp>

namespace chomper {
class Collectibles {
  public:
	explicit Collectibles(le::ITexture const& texture);

	void spawn(Player const& player);

	void draw(le::IRenderer& renderer) const;

	void eraseInstance(std::size_t index) {
		if (index >= m_sprites.instances.size()) {
			return;
		}
		m_sprites.instances.erase(m_sprites.instances.begin() + static_cast<std::vector<le::RenderInstance>::difference_type>(index));
	}

	[[nodiscard]] std::span<le::RenderInstance const> getInstances() const {
		return m_sprites.instances;
	}

  private:
	void findEmptyTiles(Player const& player);

	std::vector<int> m_emptyTiles{};
	le::Random m_random{};

	le::drawable::InstancedSprite m_sprites{};
};
} // namespace chomper