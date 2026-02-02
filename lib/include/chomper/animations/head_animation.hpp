#pragma once
#include "chomper/animator.hpp"
#include "chomper/controller.hpp"
#include "klib/base_types.hpp"
#include "klib/log.hpp"
#include "klib/ptr.hpp"
#include "le2d/drawable/sprite.hpp"
#include "le2d/render_instance.hpp"
#include <glm/vec2.hpp>

namespace chomper {
class Engine;

namespace animation {
class DirectionProvider : public klib::Polymorphic {
  public:
	[[nodiscard]] virtual glm::vec2 getHeadPosition() const = 0;
	[[nodiscard]] virtual Heading getHeading() const = 0;
};

class CollectibleProvider : public klib::Polymorphic {
  public:
	[[nodiscard]] virtual std::span<le::RenderInstance const> getCollectibles() const = 0;
};

class Eye {
  public:
	explicit Eye(le::ITexture& eyeLidTexture);
	void setPosition(glm::vec2 position);
	void movePupil(glm::vec2 target);
	void draw(le::IRenderer& renderer, bool drawEyeLids) const;

  private:
	le::drawable::Sprite m_eyeLid{};
	le::drawable::Circle m_eye{};
	le::drawable::Circle m_pupil{};
};

class HeadAnimation : public IAnimation {
  public:
	explicit HeadAnimation(klib::Ptr<DirectionProvider const> directionProvider, klib::Ptr<CollectibleProvider const> collectibleProvider,
						   gsl::not_null<Engine const*> engine);
	void tick(kvf::Seconds dt) final;
	void draw(le::IRenderer& renderer) const final;

  private:
	klib::TypedLogger<HeadAnimation> m_log;

	klib::Ptr<DirectionProvider const> m_directionProvider{};
	klib::Ptr<CollectibleProvider const> m_collectibleProvider{};

	le::drawable::Sprite m_mouth{};
	std::unique_ptr<Eye> m_leftEye{};
	std::unique_ptr<Eye> m_rightEye{};
	klib::Ptr<le::ITexture> m_eyeLidTexture{};

	bool m_shouldDraw{};
};
} // namespace animation
} // namespace chomper
