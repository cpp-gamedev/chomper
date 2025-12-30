#pragma once
#include <glm/vec2.hpp>
#include <klib/log.hpp>
#include <le2d/vsync.hpp>
#include <optional>

namespace chomper {
class Prefs {
  public:
	static constexpr auto defaultPath_v = std::string_view{"chomper_prefs.json"};

	explicit Prefs(std::string_view customPath = {});

	[[nodiscard]] std::string const& getPath() const {
		return m_path;
	}

	[[nodiscard]] std::optional<le::Vsync> getVsync() const {
		return m_vsync;
	}
	void setVsync(std::optional<le::Vsync> vsync);

	[[nodiscard]] std::optional<glm::ivec2> getWindowSize() const {
		return m_windowSize;
	}
	void setWindowSize(std::optional<glm::ivec2> windowSize);

	void reload();
	void save();

  private:
	template <typename Type>
	void overwrite(std::optional<Type>& out, std::optional<Type> const& with);

	klib::TypedLogger<Prefs> m_log{};

	std::string m_path{};

	std::optional<le::Vsync> m_vsync{};
	std::optional<glm::ivec2> m_windowSize{};
};
} // namespace chomper
