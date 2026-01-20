#include "chomper/animator.hpp"

namespace chomper::animation {
class DeathAnimation : public chomper::IAnimation {
  public:
	DeathAnimation(std::span<le::RenderInstance const> instances);
	void tick(kvf::Seconds dt) final;
	void draw(le::IRenderer& renderer) const final;

  private:
	struct Segment {
		float remaining{};
		float lifetime{};
		float rotSpeed{};
		glm::vec2 velocity{};
		le::drawable::Quad quad{};
	};

	std::vector<Segment> m_segments{};

	le::Random m_random{};
};
} // namespace chomper::animation