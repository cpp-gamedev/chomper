#include "chomper/animations/head_animation.hpp"
#include "chomper/engine.hpp"
#include "chomper/world_space.hpp"
#include "klib/enum_array.hpp"

namespace chomper::animation {
namespace {
constexpr auto isAdjacent(glm::ivec2 const& grid1, glm::ivec2 const& grid2) {
	auto dx = klib::abs(grid1.x - grid2.x);
	auto dy = klib::abs(grid1.y - grid2.y);

	return (dx <= 1 && dy <= 1) && (dx != 0 || dy != 0);
}

std::optional<glm::vec2> findAdjacentCollectible(std::span<le::RenderInstance const> collectibles, glm::ivec2 headPos) {
	auto it = std::ranges::find_if(collectibles, [&](auto const& c) {
		return isAdjacent(headPos, worldSpace::worldToGrid(c.transform.position));
	});
	if (it == collectibles.end()) {
		return std::nullopt;
	}
	return it->transform.position;
}

} // namespace

HeadAnimation::HeadAnimation(gsl::not_null<DirectionProvider const*> directionProvider, gsl::not_null<CollectibleProvider const*> collectibleProvider,
							 gsl::not_null<Engine const*> engine)
	: m_directionProvider(directionProvider), m_collectibleProvider(collectibleProvider) {
	m_mouth.set_base_size(tileSize_v);
	m_mouth.set_texture(engine->getResources().load<le::ITexture>("images/snake_mouth.png"));

	m_eyeLidTexture = engine->getResources().load<le::ITexture>("images/eye_lid.png");
	if (!m_eyeLidTexture) {
		return;
	}
	m_leftEye = std::make_unique<Eye>(*m_eyeLidTexture);
	m_rightEye = std::make_unique<Eye>(*m_eyeLidTexture);
}

void HeadAnimation::tick(kvf::Seconds /*dt*/) {
	auto headPos = m_directionProvider->getHeadPosition();
	auto heading = m_directionProvider->getHeading();

	auto c = std::cos(headingToRot_v[heading]);
	auto s = std::sin(headingToRot_v[heading]);
	auto rotation = glm::mat2(c, -s, s, c);

	auto rightEyeOffset = rotation * glm::vec2{tileSize_v.x * 0.22f, tileSize_v.y * -0.22f};
	m_rightEye->setPosition(headPos + rightEyeOffset);

	auto leftEyeOffset = rotation * glm::vec2{tileSize_v.x * -0.22f, tileSize_v.y * -0.22f};
	m_leftEye->setPosition(headPos + leftEyeOffset);

	auto target = findAdjacentCollectible(m_collectibleProvider->getCollectibles(), worldSpace::worldToGrid(headPos));
	if (!target) {
		m_shouldDraw = false;
		return;
	}
	m_shouldDraw = true;

	auto mouthOffset = rotation * glm::vec2{0.f, tileSize_v.x * 0.5f};
	m_mouth.transform.position = headPos + mouthOffset;
	m_mouth.transform.orientation = le::nvec2::from_radians(headingToRot_v[heading]);

	m_leftEye->lookAt(*target);
	m_rightEye->lookAt(*target);
}

void HeadAnimation::draw(le::IRenderer& renderer) const {
	m_leftEye->draw(renderer, !m_shouldDraw);
	m_rightEye->draw(renderer, !m_shouldDraw);

	if (!m_shouldDraw) {
		return;
	}
	m_mouth.draw(renderer);
}

Eye::Eye(le::ITexture& texture) {
	m_eye.create(tileSize_v.x * 0.4f);
	m_pupil.create(m_eye.get_diameter() * 0.5f);
	m_pupil.tint = kvf::Color{glm::vec4{0.f, 0.f, 0.f, 1.f}};
	m_eyeLid.set_base_size(m_eye.get_size());

	m_eyeLid.set_texture(&texture);
}

void Eye::setPosition(glm::vec2 position) {
	m_eyeLid.transform.position = position;
	m_eye.transform.position = position;
	m_pupil.transform.position = position;
}

void Eye::lookAt(glm::vec2 target) {
	auto dir = glm::normalize(target - m_eye.transform.position);
	m_pupil.transform.position = m_eye.transform.position + dir * m_eye.get_diameter() * 0.25f;
}

void Eye::draw(le::IRenderer& renderer, bool drawEyeLids) const {
	m_eye.draw(renderer);
	m_pupil.draw(renderer);
	if (drawEyeLids) {
		m_eyeLid.draw(renderer);
	}
}

} // namespace chomper::animation