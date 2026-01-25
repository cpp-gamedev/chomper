#pragma once
#include <string>
#include <vector>

namespace chomper {
struct AssetManifest {
	std::vector<std::string> textures{};
	std::vector<std::string> audio{};
	std::vector<std::string> fonts{};
};
} // namespace chomper
