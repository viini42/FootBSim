#include "footbsim/match_context.hpp"
#include "micro_test.hpp"

using footbsim::HomeAdvantageLevel;
using footbsim::RefereeStyle;

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
