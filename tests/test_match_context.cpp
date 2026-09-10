#include "footbsim/match_context.hpp"

#include <catch2/catch_test_macros.hpp>

using footbsim::HomeAdvantageLevel;
using footbsim::RefereeStyle;
using footbsim::TravelFatigueLevel;

TEST_CASE("home advantage NONE applies no boost", "[match_context][home_advantage]")
{
  CHECK(footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::NONE) == 1.0);
}

TEST_CASE("home advantage multiplier increases with level", "[match_context][home_advantage]")
{
  const double none = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::NONE);
  const double slight = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::SLIGHT);
  const double moderate = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::MODERATE);
  const double strong = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::STRONG);
  const double maximum = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::MAXIMUM);

  CHECK(none < slight);
  CHECK(slight < moderate);
  CHECK(moderate < strong);
  CHECK(strong < maximum);
}

TEST_CASE("referee style BALANCED applies no adjustment", "[match_context][referee]")
{
  CHECK(footbsim::RefereeStrictnessMultiplier(RefereeStyle::BALANCED) == 1.0);
}

TEST_CASE("referee strictness multiplier increases with style", "[match_context][referee]")
{
  const double lenient = footbsim::RefereeStrictnessMultiplier(RefereeStyle::LENIENT);
  const double balanced = footbsim::RefereeStrictnessMultiplier(RefereeStyle::BALANCED);
  const double strict = footbsim::RefereeStrictnessMultiplier(RefereeStyle::STRICT);
  const double very_strict = footbsim::RefereeStrictnessMultiplier(RefereeStyle::VERY_STRICT);

  CHECK(lenient < balanced);
  CHECK(balanced < strict);
  CHECK(strict < very_strict);
}

TEST_CASE("travel fatigue SAME_CITY applies no penalty", "[match_context][travel_fatigue]")
{
  CHECK(footbsim::TravelFatiguePenalty(TravelFatigueLevel::SAME_CITY) == 0.0);
}

TEST_CASE("travel fatigue penalty increases with distance", "[match_context][travel_fatigue]")
{
  // A different state (a neighboring state, same region) is a shorter trip
  // than a different region (which spans multiple states), so the penalty
  // ordering is state < region here, not alphabetical/declaration order.
  const double same_city = footbsim::TravelFatiguePenalty(TravelFatigueLevel::SAME_CITY);
  const double state = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_STATE);
  const double region = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_REGION);
  const double country = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_COUNTRY);

  CHECK(same_city < state);
  CHECK(state < region);
  CHECK(region < country);
}

TEST_CASE("travel fatigue stays a small nudge even at its largest", "[match_context][travel_fatigue]")
{
  // This is meant as light seasoning on the simulation, not a dominant
  // factor -- guard against it creeping up into something that swings
  // matches on its own.
  const double country = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_COUNTRY);
  CHECK(country <= 15.0);
}
