#pragma once
#include "chomper/debug_inspectable.hpp"
#include <klib/c_string.hpp>
#include <kvf/color.hpp>
#include <gsl/pointers>
#include <span>

namespace chomper {
struct InspectItem {
	gsl::not_null<IDebugInspectable*> inspectable;
	klib::CString label{};
};

namespace im_util {
void inspectAsTabs(std::span<InspectItem const> items);

void textColored(kvf::Color color, klib::CString text);
} // namespace im_util
} // namespace chomper
