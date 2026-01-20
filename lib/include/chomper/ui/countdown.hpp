#pragma once
#include "le2d/drawable/shape.hpp"
#include "le2d/drawable/text.hpp"
#include <kvf/time.hpp>

namespace chomper::ui {
class Countdown {
  public:
	static constexpr auto textHeight_v = le::TextHeight{120};

	explicit Countdown(gsl::not_null<le::IFont*> font, le::TextHeight textHeight = textHeight_v, kvf::Seconds timer = 3s);

	[[nodiscard]] auto getRemain() const -> kvf::Seconds {
		return m_remain;
	}

	void tick(kvf::Seconds dt);
	void draw(le::IRenderer& renderer) const;

  private:
	void setTimerText(std::chrono::seconds value);
	void updateSector();

	gsl::not_null<le::IFont*> m_font;
	le::TextHeight m_textHeight{120};

	le::drawable::Sector m_sector{};
	le::drawable::Circle m_background{};
	le::drawable::Text m_text{};

	kvf::Seconds m_timer{};
	kvf::Seconds m_remain{};
};
} // namespace chomper::ui
