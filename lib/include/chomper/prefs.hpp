#pragma once
#include <glm/vec2.hpp>
#include <klib/log.hpp>
#include <le2d/vsync.hpp>
#include <optional>

namespace chomper {
class Prefs {
  public:
	inline static auto const defaultPath = std::string{"chomper_prefs.json"};

	explicit Prefs(std::string_view customPath = {});

	[[nodiscard]] std::string const& getPath() const {
		return m_path;
	}

	[[nodiscard]] std::optional<le::Vsync> getVsync() const {
		return m_vsync;
	}

	void setVsync(std::optional<le::Vsync> vsync);

	void reload();
	void save();

  private:
	klib::TypedLogger<Prefs> m_log{};

	std::string m_path{};

	std::optional<le::Vsync> m_vsync{};
};
} // namespace chomper
