#include "chomper/animations/death_animation.hpp"
#include "chomper/world_space.hpp"
#include <numbers>

namespace chomper::animation {
namespace {
constexpr auto easeOut(float t) {
	return 1.f - ((1.f - t) * (1.f - t));
}
} // namespace

DeathAnimation::DeathAnimation(std::span<le::RenderInstance const> instances) {
	m_segments.clear();
	m_segments.reserve(instances.size());
	m_quads.instances.clear();
	m_quads.instances.reserve(instances.size());
	m_quads.create(tileSize_v);

	for (auto const& instance : instances) {
		auto dir = m_random.next_float(0, 360);
		auto lifetime = m_random.next_float(2, 5);
		auto speed = m_random.next_float(300, 600);
		auto rotSpeed = m_random.next_float(5.f, 20.f);

		auto rad = dir * std::numbers::pi_v<float> / 180.f;

		Segment seg;
		seg.rotSpeed = rotSpeed;
		seg.velocity = {std::cos(rad) * speed, std::sin(rad) * speed};
		seg.remaining = lifetime;
		seg.lifetime = lifetime;

		m_quads.instances.emplace_back().tint = instance.tint;
		m_quads.instances.back().transform = instance.transform;

		m_segments.push_back(seg);
	}
}

void DeathAnimation::tick(kvf::Seconds dt) {
	assert(m_segments.size() == m_quads.instances.size());

	auto animating = false;

	for (std::size_t i = 0; i < m_segments.size(); i++) {
		auto& seg = m_segments[i];
		if (seg.remaining <= 0.f) {
			continue;
		}

		seg.remaining -= dt.count();

		auto t = std::clamp(1.f - (seg.remaining / seg.lifetime), 0.f, 1.f);

		auto eased = 1.f - easeOut(t);

		auto& quad = m_quads.instances.at(i);
		quad.transform.position += seg.velocity * eased * dt.count();
		if (worldSpace::isOutOfBounds(worldSpace::worldToGrid(quad.transform.position))) {
			seg.velocity = -seg.velocity;
		}

		auto angle = dt.count() * eased * seg.rotSpeed;
		quad.transform.orientation.rotate(angle);

		if (t > 0.8f) {
			auto scale = 1.f;
			auto u = (t - 0.8f) / 0.2f; // 0 → 1
			scale = 1.f - u;			// 1 → 0
			quad.transform.scale = {scale, scale};
		}

		if (t < 1.f) {
			animating = true;
		}
	}
	m_finished = !animating;
}

void DeathAnimation::draw(le::IRenderer& renderer) const {
	m_quads.draw(renderer);
}

} // namespace chomper::animation
