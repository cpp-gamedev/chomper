#include "chomper/prefs.hpp"
#include <djson/json.hpp>
#include <klib/log.hpp>
#include <filesystem>

namespace chomper {
namespace {
namespace fs = std::filesystem;

[[nodiscard]] constexpr auto to_vsync(std::string_view const text) -> std::optional<le::Vsync> {
	for (auto vsync = le::Vsync{}; vsync < le::Vsync::COUNT_; vsync = le::Vsync(int(vsync) + 1)) {
		if (text == le::vsync_str_v[vsync]) {
			return vsync;
		}
	}
	return {};
}

template <typename Type>
void from_table(dj::Json const& json, std::string_view const key, std::optional<Type>& out_data) {
	if (!json.as_object().contains(key)) {
		return;
	}
	out_data.emplace();
	from_json(json[key], *out_data);
}

template <typename Type>
void to_table(dj::Json& json, std::string_view const key, std::optional<Type> const& data) {
	if (data) {
		to_json(json[key], *data);
	}
}

void from_table(dj::Json const& json, std::string_view const key, std::optional<le::Vsync>& out_vsync) {
	if (!json.as_object().contains(key)) {
		return;
	}
	out_vsync = to_vsync(json[key].as_string_view());
}

void to_table(dj::Json& json, std::string_view const key, std::optional<le::Vsync> const vsync) {
	if (!vsync) {
		return;
	}
	to_json(json[key], le::vsync_str_v[*vsync]);
}

namespace key {
constexpr std::string_view vsync_v{"vsync"};
} // namespace key
} // namespace

Prefs::Prefs(std::string_view const customPath) {
	m_path = customPath.empty() ? defaultPath : customPath;
	reload();
}

void Prefs::setVsync(std::optional<le::Vsync> const vsync) {
	if (m_vsync == vsync) {
		return;
	}

	m_vsync = vsync;
	save();
}

void Prefs::reload() {
	if (!fs::is_regular_file(m_path)) {
		m_log.info("no Prefs found at path: '{}'", m_path);
		return;
	}

	auto json = dj::Json::from_file(m_path);
	if (!json) {
		m_log.warn("failed to read Prefs from path: '{}'", m_path);
		return;
	}

	from_table(*json, key::vsync_v, m_vsync);

	m_log.info("Prefs loaded from '{}'", m_path);
}

void Prefs::save() {
	if (m_path.empty()) {
		m_log.warn("failed to save Prefs, invalid path");
		return;
	}

	auto json = dj::Json{};
	to_table(json, key::vsync_v, m_vsync);

	if (!json.to_file(m_path)) {
		m_log.warn("failed to save Prefs to path: '{}'", m_path);
		return;
	}

	m_log.info("Prefs saved to '{}'", m_path);
}
} // namespace chomper
