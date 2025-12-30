#pragma once
#include <klib/base_types.hpp>

namespace chomper {
class IDebugInspector : public klib::Polymorphic {
  public:
	virtual void debugInspect() = 0;
};
} // namespace chomper
