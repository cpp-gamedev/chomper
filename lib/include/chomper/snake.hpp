#pragma once
#include "chomper/controller.hpp"
#include <klib/log.hpp>
#include <kvf/time.hpp>
#include <le2d/drawable/shape.hpp>
#include <deque>

namespace chomper {
class Snake {
  public:
	explicit Snake();
	void tick(kvf::Seconds dt);
	void draw(le::IRenderer& renderer) const;

	void setHeading(Heading heading);
	void debugInspect();

  private:
	void popTail();
	void grow();

	klib::TypedLogger<Snake> m_log{};

	Heading m_heading{};
	std::vector<Heading> m_headingQueue{};

	std::deque<le::RenderInstance> m_instances{};
	le::drawable::InstancedQuad m_quads{};

	kvf::Seconds m_moveTimer{};
	// bool to decide wether to remove the tail, turn false if the snake has eaten
	bool m_shouldPop = true;
};

} // namespace chomper