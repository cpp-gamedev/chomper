#pragma once
#include <le2d/drawable/shape.hpp>

namespace chomper::ui {
class ProgressBar {
  public:
	void setProgress(float value);

	void draw(le::IRenderer& renderer) const {
		quad.draw(renderer);
	}

	le::drawable::Quad quad{};

	glm::vec2 targetSize{300.0f, 50.0f};
};
} // namespace chomper::ui
