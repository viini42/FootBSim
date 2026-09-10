#include "footbsim/match_context.hpp"
#include "micro_test.hpp"

using footbsim::HomeAdvantageLevel;

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
