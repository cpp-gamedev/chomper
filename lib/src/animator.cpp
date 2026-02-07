#include "chomper/animator.hpp"

namespace chomper {
void Animator::play(std::unique_ptr<IAnimation> animation) {
	m_playing.emplace_back(std::move(animation));
}

void Animator::tick(kvf::Seconds dt) {
	std::erase_if(m_playing, [&](auto const& animation) {
		return animation->finished();
	});
	for (auto const& animation : m_playing) {
		animation->tick(dt);
	}
}

void Animator::stopAll() {
	m_playing.clear();
}

void Animator::draw(le::IRenderer& renderer) const {
	for (auto const& animation : m_playing) {
		animation->draw(renderer);
	}
}
} // namespace chomper