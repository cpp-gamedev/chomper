#include "chomper/ui/progress_bar.hpp"
#include <algorithm>

namespace chomper::ui {
void ProgressBar::setProgress(float value) {
	value = std::clamp(value, 0.0f, 1.0f);
	auto const size = glm::vec2{value * targetSize.x, targetSize.y};
	auto const xOffset = 0.5f * size.x;
	auto const rect = kvf::Rect<>::from_size(size, {xOffset, 0.0f});
	quad.create(rect);
}
} // namespace chomper::ui
