#pragma once
#include <kvf/time.hpp>
#include <le2d/drawable/shape.hpp>
#include <le2d/random.hpp>
#include <le2d/render_instance.hpp>

namespace chomper {
class IAnimation : public klib::Polymorphic {
  public:
	virtual void tick(kvf::Seconds dt) = 0;
	virtual void draw(le::IRenderer& renderer) const = 0;
};

class Animator {
  public:
	void play(std::unique_ptr<IAnimation> animation);

	void tick(kvf::Seconds dt);

	void draw(le::IRenderer& renderer) const;

  private:
	std::vector<std::unique_ptr<IAnimation>> m_playing{};
};
} // namespace chomper