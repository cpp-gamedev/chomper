#include "chomper/runtimes/entrypoint.hpp"
#include "chomper/runtimes/game.hpp"
#include <klib/visitor.hpp>

namespace chomper::runtime {
Entrypoint::Entrypoint(gsl::not_null<Engine*> engine) : m_engine(engine) {
	static constexpr auto textParams_v = le::drawable::Text::Params{
		.height = le::TextHeight{60},
	};
	m_mainText.set_string(engine->getResources().getMainFont(), "START", textParams_v);
}

void Entrypoint::tick(kvf::Seconds const dt) {
	swingMainText(dt);

	auto const visitor = klib::SubVisitor{[this](le::event::Key const& key) {
		if (key.action != GLFW_PRESS) {
			return;
		}
		m_engine->setNextRuntime<Game>();
	}};
	for (auto const& event : m_engine->getContext().event_queue()) {
		std::visit(visitor, event);
	}
}

void Entrypoint::render(le::IRenderer& renderer) const {
	m_mainText.draw(renderer);
}

void Entrypoint::swingMainText(kvf::Seconds const dt) {
	static constexpr auto speed_v = 5.0f;
	static constexpr auto amplitude_v = 20.0f;
	m_elapsed += dt;
	auto const offset = glm::sin(speed_v * m_elapsed.count()) * amplitude_v;
	m_mainText.transform.position.x = offset;
}
} // namespace chomper::runtime
