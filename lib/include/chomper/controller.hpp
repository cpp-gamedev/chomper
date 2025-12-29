#pragma once
#include <klib/base_types.hpp>
#include <klib/enum_array.hpp>
#include <kvf/time.hpp>
#include <cstdint>
#include <gsl/pointers>
#include <string_view>

namespace chomper {
// represents a direction.
enum class Heading : std::int8_t {
	East,
	North,
	West,
	South,
	COUNT_
};
constexpr auto heading_name_v = klib::EnumArray<Heading, std::string_view>{"East", "North", "West", "South"};

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
