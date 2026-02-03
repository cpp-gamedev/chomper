#pragma once
#include "chomper/heading.hpp"
#include <klib/base_types.hpp>
#include <klib/enum_array.hpp>
#include <kvf/time.hpp>
#include <gsl/pointers>

namespace chomper {
// Heading controller interface.
class IController : public klib::Polymorphic {
  public:
	class IListener;

	explicit IController(gsl::not_null<IListener*> listener) : m_listener(listener) {}

	virtual void tick(kvf::Seconds dt) = 0;

  protected:
	// observer for this controller instance, should be set to the owner (avoids dangling).
	gsl::not_null<IListener*> m_listener;
};

// Heading controller listener interface.
class IController::IListener : public klib::Polymorphic {
  public:
	// called whenever observed controller decides to set the heading.
	virtual void onSetHeading(Heading heading) = 0;
};
} // namespace chomper
