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
		std::string_view assetsDir{};
		bool noLibdecor{};
	};

	explicit Engine(CreateInfo const& createInfo);

	[[nodiscard]] le::IDataLoader const& getDataLoader() const {
		return *m_dataLoader;
	}
	[[nodiscard]] le::Context const& getContext() const {
		return *m_context;
	}
	[[nodiscard]] le::input::Router const& getInputRouter() const {
		return m_inputRouter;
	}
	[[nodiscard]] le::input::Router& getInputRouter() {
		return m_inputRouter;
	}

	void run();

  private:
	void createDataLoader(std::string_view assetsDir);
	void createContext(CreateInfo const& createInfo);
	void createRuntime();

	klib::TypedLogger<Engine> m_log{};

	std::unique_ptr<le::IDataLoader> m_dataLoader{};
	std::unique_ptr<le::Context> m_context{};
	le::input::Router m_inputRouter{};

	std::unique_ptr<IRuntime> m_runtime{};
};
} // namespace chomper
