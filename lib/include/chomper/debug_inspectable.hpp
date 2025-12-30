#pragma once
#include <klib/base_types.hpp>

namespace chomper {
class IDebugInspectable : public klib::Polymorphic {
  public:
	virtual void debugInspect() = 0;
};
} // namespace chomper
