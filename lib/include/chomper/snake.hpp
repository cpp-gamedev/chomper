#pragma once
#include "chomper/controller.hpp"
#include "chomper/world_size.hpp"
#include <klib/log.hpp>
#include <kvf/time.hpp>
#include <le2d/drawable/shape.hpp>
#include <deque>

namespace chomper {
class Snake {
  public:
	explicit Snake();
	void draw(le::IRenderer& renderer) const;

	void grow(Heading heading);
	void popTail();

	[[nodiscard]] std::span<le::RenderInstance const> getSegments() const {
		return m_quads.instances;
	}

  private:
	void syncQuads();

	std::deque<le::RenderInstance> m_instances{};
	le::drawable::InstancedQuad m_quads{tileSize_v};

	size_t m_baseSize{6};
};

} // namespace chomper
