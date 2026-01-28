#include "chomper/collectibles.hpp"
#include "chomper/world_size.hpp"
#include "chomper/world_space.hpp"
#include "le2d/renderer.hpp"

namespace chomper {
namespace {
constexpr auto collectibleAmount_v = 10;
}

Collectibles::Collectibles(le::ITexture const& texture) {
	m_sprites.set_base_size(tileSize_v);
	m_sprites.set_texture(&texture);
}

void Collectibles::spawn(Player const& player) {
	findEmptyTiles(player);

	for (auto i = m_sprites.instances.size(); i < collectibleAmount_v; i++) {
		if (m_emptyTiles.empty()) {
			return;
		}
		// find a random tile
		auto random = m_random.next_index(m_emptyTiles.size());
		auto tile = m_emptyTiles[random];
		// remove said tile from the vector
		std::erase_if(m_emptyTiles, [&](auto const& v) {
			return v == m_emptyTiles[random];
		});
		// place the collectible on the tile
		auto width = worldSize_v.x;
		m_sprites.instances.emplace_back();
		m_sprites.instances.back().transform.position = worldSpace::gridToWorld({tile % width, tile / width});
	}
}

void Collectibles::findEmptyTiles(Player const& player) {
	m_emptyTiles.clear();
	m_emptyTiles.reserve(worldSize_v.x * worldSize_v.y);
	for (auto i = 0; i < worldSize_v.x * worldSize_v.y; i++) {
		m_emptyTiles.push_back(i);
	}

	auto const removeTile = [this](int tile) {
		auto it = std::ranges::find(m_emptyTiles, tile);
		if (it != m_emptyTiles.end()) {
			*it = m_emptyTiles.back();
			m_emptyTiles.pop_back();
		}
	};

	for (auto const& seg : player.getSegments()) {
		auto p = worldSpace::worldToGrid(seg.transform.position);
		removeTile((p.y * worldSize_v.x) + p.x);
	}

	for (auto const& c : m_sprites.instances) {
		auto p = worldSpace::worldToGrid(c.transform.position);
		removeTile((p.y * worldSize_v.x) + p.x);
	}
}

void Collectibles::draw(le::IRenderer& renderer) const {
	m_sprites.draw(renderer);
}
} // namespace chomper