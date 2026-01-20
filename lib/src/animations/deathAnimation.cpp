#include "chomper/animations/deathAnimation.hpp"
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
		seg.quad.create(tileSize_v);
		seg.quad.tint = instance.tint;
		seg.quad.transform = instance.transform;
		m_segments.push_back(seg);
	}
}
void DeathAnimation::tick(kvf::Seconds dt) {
	for (auto& seg : m_segments) {
		if (seg.remaining <= 0.f) {
			continue;
		}

		seg.remaining -= dt.count();

		auto t = std::clamp(1.f - (seg.remaining / seg.lifetime), 0.f, 1.f);

		auto eased = 1.f - easeOut(t);

		seg.quad.transform.position += seg.velocity * eased * dt.count();
		if (worldSpace::isOutOfBounds(worldSpace::worldToGrid(seg.quad.transform.position))) {
			seg.velocity = -seg.velocity;
		}

		float angle = dt.count() * eased * seg.rotSpeed;
		auto o = seg.quad.transform.orientation;
		auto cos = std::cos(angle);
		auto sin = std::sin(angle);
		seg.quad.transform.orientation = {(o.x * cos) - (o.y * sin), (o.x * sin) + (o.y * cos)};

		float scale = 1.f;
		if (t > 0.8f) {
			float u = (t - 0.8f) / 0.2f; // 0 → 1
			scale = 1.f - u;			 // 1 → 0
		}
		seg.quad.transform.scale = {scale, scale};
	}
}

void DeathAnimation::draw(le::IRenderer& renderer) const {
	for (auto const& segment : m_segments) {
		if (segment.remaining > 0.f) {
			segment.quad.draw(renderer);
		}
	}
}

} // namespace chomper::animation
