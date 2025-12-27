#pragma once
#include "chomper/logger.hpp"
#include <le2d/context.hpp>
#include <le2d/data_loader.hpp>
#include <memory>

namespace chomper {
class Engine {
  public:
	struct CreateInfo {
		std::string_view assets_dir{};
		bool no_libdecor{};
	};

	explicit Engine(CreateInfo create_info);

	[[nodiscard]] auto get_data_loader() const -> le::IDataLoader const& { return *m_data_loader; }

	[[nodiscard]] auto get_context() const -> le::Context& { return *m_context; }

	void run();

  private:
	Logger<Engine> m_log{};

	std::unique_ptr<le::IDataLoader> m_data_loader{};
	std::unique_ptr<le::Context> m_context{};
};
} // namespace chomper
