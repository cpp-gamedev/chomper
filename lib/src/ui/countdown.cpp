#include "chomper/ui/countdown.hpp"
#include "chomper/theme.hpp"
#include <klib/assert.hpp>

namespace chomper::ui {
Countdown::Countdown(gsl::not_null<le::IFont*> font, le::TextHeight const textHeight, kvf::Seconds const timer)
	: m_font(font), m_textHeight(textHeight), m_timer(timer), m_remain(timer) {
	auto const diameter = float(m_textHeight) + 30.0f;
	m_background.create(diameter);
	m_background.tint = theme::clearColor_v;
}

void Countdown::tick(kvf::Seconds const dt) {
	if (m_remain <= 0s) {
		return;
	}

	auto const prevSeconds = std::chrono::duration_cast<std::chrono::seconds>(m_remain);
	m_remain -= dt;
	auto const currSeconds = std::chrono::duration_cast<std::chrono::seconds>(m_remain);
	if (prevSeconds > currSeconds) {
		setTimerText(currSeconds + 1s);
	}

	updateSector();
}

void Countdown::draw(le::IRenderer& renderer) const {
	if (m_remain <= 0s) {
		return;
	}

	m_sector.draw(renderer);
	m_background.draw(renderer);
	m_text.draw(renderer);
}

void Countdown::setTimerText(std::chrono::seconds const value) {
	auto const params = le::drawable::Text::Params{
		.height = m_textHeight,
		.expand = le::drawable::TextExpand::eBoth,
	};
	auto const text = std::format("{}", value.count());
	m_text.set_string(*m_font, text, params);

	// technically this isn't correct y-centering because parts of glyphs can be above/below the baseline,
	// and Text::get_size() is insufficient to adjust for that.
	// here, since each displayed glyph (0-9) is entirely above the baseline (unlike say 'g'),
	// it can be pushed down by half the size and it will "look" consistently y-centered.
	// this is what is known as a "hack".
	m_text.transform.position.y = -0.5f * m_text.get_size().y;
}

void Countdown::updateSector() {
	KLIB_ASSERT(m_timer > 0s);
	auto const ratio = m_remain / m_timer;
	static constexpr auto degreesBegin_v{90.0f};
	auto const degreesEnd = degreesBegin_v + (ratio * 360.0f);
	auto const diameter = m_background.get_diameter() + 30.0f;
	auto const sectorParams = le::shape::Sector::Params{
		.degrees_begin = degreesBegin_v,
		.degrees_end = degreesEnd,
	};
	m_sector.create(diameter, sectorParams);
}
} // namespace chomper::ui
