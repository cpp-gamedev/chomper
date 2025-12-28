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
	auto engine_ci = Engine::CreateInfo{};
	auto const parse_info = klib::args::ParseInfo{.version = build_version_str};
	auto const args = std::array{
		klib::args::named_option(engine_ci.assets_dir, "a,assets", "override assets directory"),
		klib::args::named_flag(engine_ci.no_libdecor, "no-libdecor", "disable libdecor (ignored unless Wayland)"),
	};
	auto const parse_result = klib::args::parse_main(parse_info, args, argc, argv);
	if (parse_result.early_return()) { return parse_result.get_return_code(); }

	log.info("chomper {}", chomper::build_version_str);

	auto engine = Engine{engine_ci};
	engine.run();

	log.info("shutting down");

	return EXIT_SUCCESS;
}
} // namespace
} // namespace chomper

auto main(int argc, char** argv) -> int {
	auto const log_file = klib::log::File{"chomper.log"};
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
