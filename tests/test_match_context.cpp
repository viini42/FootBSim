#include "footbsim/match_context.hpp"
#include "micro_test.hpp"

using footbsim::HomeAdvantageLevel;
using footbsim::RefereeStyle;
using footbsim::TravelFatigueLevel;

FOOTBSIM_TEST(home_advantage_none_applies_no_boost)
{
  FOOTBSIM_CHECK(footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::NONE) == 1.0);
}

FOOTBSIM_TEST(home_advantage_multiplier_increases_with_level)
{
  const double none = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::NONE);
  const double slight = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::SLIGHT);
  const double moderate = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::MODERATE);
  const double strong = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::STRONG);
  const double maximum = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::MAXIMUM);

  FOOTBSIM_CHECK(none < slight);
  FOOTBSIM_CHECK(slight < moderate);
  FOOTBSIM_CHECK(moderate < strong);
  FOOTBSIM_CHECK(strong < maximum);
}

FOOTBSIM_TEST(referee_style_balanced_applies_no_adjustment)
{
  FOOTBSIM_CHECK(footbsim::RefereeStrictnessMultiplier(RefereeStyle::BALANCED) == 1.0);
}

FOOTBSIM_TEST(referee_strictness_multiplier_increases_with_style)
{
  const double lenient = footbsim::RefereeStrictnessMultiplier(RefereeStyle::LENIENT);
  const double balanced = footbsim::RefereeStrictnessMultiplier(RefereeStyle::BALANCED);
  const double strict = footbsim::RefereeStrictnessMultiplier(RefereeStyle::STRICT);
  const double very_strict = footbsim::RefereeStrictnessMultiplier(RefereeStyle::VERY_STRICT);

  FOOTBSIM_CHECK(lenient < balanced);
  FOOTBSIM_CHECK(balanced < strict);
  FOOTBSIM_CHECK(strict < very_strict);
}

FOOTBSIM_TEST(travel_fatigue_same_city_applies_no_penalty)
{
  FOOTBSIM_CHECK(footbsim::TravelFatiguePenalty(TravelFatigueLevel::SAME_CITY) == 0.0);
}

FOOTBSIM_TEST(travel_fatigue_penalty_increases_with_distance)
{
  // A different state (a neighboring state, same region) is a shorter trip
  // than a different region (which spans multiple states), so the penalty
  // ordering is state < region here, not alphabetical/declaration order.
  const double same_city = footbsim::TravelFatiguePenalty(TravelFatigueLevel::SAME_CITY);
  const double state = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_STATE);
  const double region = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_REGION);
  const double country = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_COUNTRY);

  FOOTBSIM_CHECK(same_city < state);
  FOOTBSIM_CHECK(state < region);
  FOOTBSIM_CHECK(region < country);
}

FOOTBSIM_TEST(travel_fatigue_stays_a_small_nudge_even_at_its_largest)
{
  // This is meant as light seasoning on the simulation, not a dominant
  // factor -- guard against it creeping up into something that swings
  // matches on its own.
  const double country = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_COUNTRY);
  FOOTBSIM_CHECK(country <= 15.0);
}
