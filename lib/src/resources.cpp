#include "chomper/resources.hpp"

namespace chomper {
Resources::Resources(le::AssetLoader assetLoader) : m_assetLoader(std::move(assetLoader)), m_mainFont(&loadRequired<le::IFont>("fonts/main.ttf")) {}
} // namespace chomper
