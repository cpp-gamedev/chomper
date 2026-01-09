#pragma once
#include "chomper/debug_inspector.hpp"
#include "chomper/debug_stats.hpp"
#include "chomper/prefs.hpp"
#include "chomper/resources.hpp"
#include "chomper/runtime.hpp"
#include <klib/log.hpp>
#include <le2d/context.hpp>
#include <le2d/data_loader.hpp>
#include <le2d/input/router.hpp>
#include <functional>
#include <memory>

namespace chomper {
class Engine : public IDebugInspector, public klib::Pinned {
  public:
	struct CreateInfo {
		std::string_view assetsDir{};
		std::string_view prefsPath{};
		bool noLibdecor{};
	};

	using CreateRuntime = std::move_only_function<std::unique_ptr<IRuntime>(Engine&)>;

	explicit Engine(CreateInfo const& createInfo);

	[[nodiscard]] le::IDataLoader const& getDataLoader() const {
		return *m_dataLoader;
	}
	[[nodiscard]] le::Context const& getContext() const {
		return *m_context;
	}
	[[nodiscard]] Resources& getResources() const {
		return *m_resources;
	}
	[[nodiscard]] le::input::Router const& getInputRouter() const {
		return m_inputRouter;
	}
	[[nodiscard]] le::input::Router& getInputRouter() {
		return m_inputRouter;
	}

	[[nodiscard]] Prefs const& getPrefs() const {
		return m_prefs;
	}
	[[nodiscard]] Prefs& getPrefs() {
		return m_prefs;
	}

	[[nodiscard]] DebugStats const& getDebugStats() const {
		return m_debugStats;
	}

	void run();

	void setVsync(le::Vsync vsync);
	void setNextRuntime(CreateRuntime callback);

	template <std::derived_from<IRuntime> RuntimeT>
		requires(std::constructible_from<RuntimeT, Engine*>)
	void setNextRuntime() {
		setNextRuntime([](Engine& self) {
			return std::make_unique<RuntimeT>(&self);
		});
	}

  private:
	struct RuntimeState {
		float dtScale{1.0f};
		bool wireframe{false};
	};

	// IDebugInspector
	void debugInspect() final;

	void createDataLoader(std::string_view assetsDir);
	void createContext(CreateInfo const& createInfo);
	void createResources();

	void inspectStats();
	void inspectVsync();
	void inspectDtScale();

	void checkNextRuntime();
	void processEvents();

	klib::TypedLogger<Engine> m_log{};
	Prefs m_prefs{};

	std::unique_ptr<le::IDataLoader> m_dataLoader{};
	std::unique_ptr<le::Context> m_context{};
	std::unique_ptr<Resources> m_resources{};
	le::input::Router m_inputRouter{};

	std::unique_ptr<IRuntime> m_runtime{};
	CreateRuntime m_nextRuntime{};

	RuntimeState m_runtimeState{};

	DebugStats m_debugStats{};
};
} // namespace chomper
