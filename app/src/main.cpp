#include "chomper/build_version.hpp"
#include "chomper/engine.hpp"
#include <klib/args/parse.hpp>
#include <klib/assert.hpp>
#include <klib/log.hpp>
#include <array>

namespace chomper {
namespace {
auto const log = klib::TaggedLogger{"chomper"};

auto run(int const argc, char const* const* argv) -> int {
	auto engineCI = Engine::CreateInfo{};
	auto const parseInfo = klib::args::ParseInfo{.version = buildVersionStr};
	auto const args = std::array{
		klib::args::named_option(engineCI.assetsDir, "a,assets", "override assets directory"),
		klib::args::named_flag(engineCI.noLibdecor, "no-libdecor", "disable libdecor (ignored unless Wayland)"),
	};
	auto const parseResult = klib::args::parse_main(parseInfo, args, argc, argv);
	if (parseResult.early_return()) {
		return parseResult.get_return_code();
	}

	log.info("{}", chomper::buildVersionStr);

	auto engine = Engine{engineCI};
	engine.run();

	log.info("shutting down");

	return EXIT_SUCCESS;
}
} // namespace
} // namespace chomper

auto main(int argc, char** argv) -> int {
	auto const logFile = klib::log::File{"chomper.log"};
	try {
		chomper::run(argc, argv);
	} catch (std::exception const& e) {
		chomper::log.error("PANIC: {}", e.what());
		return EXIT_FAILURE;
	} catch (...) {
		chomper::log.error("PANIC!");
		return EXIT_FAILURE;
	}
}
