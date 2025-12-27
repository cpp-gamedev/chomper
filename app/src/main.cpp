#include <klib/args/parse.hpp>
#include <klib/assert.hpp>
#include <klib/log.hpp>
#include <array>
#include "chomper/build_version.hpp"
#include "chomper/engine.hpp"

namespace chomper {
namespace {
auto run(int const argc, char const* const* argv) -> int {
	auto engine_ci = Engine::CreateInfo{};
	auto const parse_info = klib::args::ParseInfo{.version = build_version_str};
	auto const args = std::array{
		klib::args::named_option(engine_ci.assets_dir, "a,assets", "override assets directory"),
		klib::args::named_flag(engine_ci.no_libdecor, "no-libdecor", "disable libdecor (ignored unless Wayland)"),
	};
	auto const parse_result = klib::args::parse_main(parse_info, args, argc, argv);
	if (parse_result.early_return()) { return parse_result.get_return_code(); }

	auto engine = Engine{engine_ci};
	engine.run();

	return EXIT_SUCCESS;
}
} // namespace
} // namespace chomper

auto main(int argc, char** argv) -> int {
	auto const log_file = klib::log::File{"space-rogue.log"};
	auto const log = klib::TaggedLogger{"chomper"};
	try {
		chomper::run(argc, argv);
	} catch (std::exception const& e) {
		log.error("PANIC: {}", e.what());
		return EXIT_FAILURE;
	} catch (...) {
		log.error("PANIC!");
		return EXIT_FAILURE;
	}
}
