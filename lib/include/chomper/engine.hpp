#pragma once
#include "chomper/runtime.hpp"
#include <klib/log.hpp>
#include <le2d/context.hpp>
#include <le2d/data_loader.hpp>
#include <le2d/input/router.hpp>
#include <memory>

namespace chomper {
class Engine : public klib::Pinned {
  public:
	struct CreateInfo {
		std::string_view assets_dir{};
		bool no_libdecor{};
	};

	explicit Engine(CreateInfo const& create_info);

	[[nodiscard]] auto get_data_loader() const -> le::IDataLoader const& { return *m_data_loader; }
	[[nodiscard]] auto get_context() const -> le::Context const& { return *m_context; }
	[[nodiscard]] auto get_input_router() const -> le::input::Router const& { return m_input_router; }
	[[nodiscard]] auto get_input_router() -> le::input::Router& { return m_input_router; }

	void run();

  private:
	void create_data_loader(std::string_view assets_dir);
	void create_context(CreateInfo const& create_info);
	void create_runtime();

	klib::TypedLogger<Engine> m_log{};

	std::unique_ptr<le::IDataLoader> m_data_loader{};
	std::unique_ptr<le::Context> m_context{};
	le::input::Router m_input_router{};

	std::unique_ptr<IRuntime> m_runtime{};
};
} // namespace chomper
