#include "footbsim/team_config.hpp"

#include <catch2/catch_test_macros.hpp>

using footbsim::Formation;
using footbsim::PressIntensity;
using footbsim::Tempo;

TEST_CASE("Formation aggression offset escalates from defensive to attacking",
          "[team_config][formation]")
{
  const double defensive = footbsim::FormationAggressionOffset(Formation::FIVE_THREE_TWO);
  const double balanced = footbsim::FormationAggressionOffset(Formation::FOUR_FOUR_TWO);
  const double possession = footbsim::FormationAggressionOffset(Formation::THREE_FIVE_TWO);
  const double attacking = footbsim::FormationAggressionOffset(Formation::FOUR_THREE_THREE);

  CHECK(balanced == 0.0);
  CHECK(defensive < balanced);
  CHECK(balanced < possession);
  CHECK(possession < attacking);
}

TEST_CASE("PressIntensity stamina cost increases with intensity, MEDIUM is a no-op",
          "[team_config][press_intensity]")
{
  const double low = footbsim::PressIntensityStaminaCost(PressIntensity::LOW_BLOCK);
  const double medium = footbsim::PressIntensityStaminaCost(PressIntensity::MEDIUM);
  const double high = footbsim::PressIntensityStaminaCost(PressIntensity::HIGH_PRESS);

  CHECK(medium == 0.0);
  CHECK(low < medium);
  CHECK(medium < high);
}

TEST_CASE("PressIntensity foul multiplier increases with intensity",
          "[team_config][press_intensity]")
{
  const double low = footbsim::PressIntensityFoulMultiplier(PressIntensity::LOW_BLOCK);
  const double medium = footbsim::PressIntensityFoulMultiplier(PressIntensity::MEDIUM);
  const double high = footbsim::PressIntensityFoulMultiplier(PressIntensity::HIGH_PRESS);

  CHECK(medium == 1.0);
  CHECK(low < medium);
  CHECK(medium < high);
}

TEST_CASE("Tempo advance multiplier increases from patient to direct", "[team_config][tempo]")
{
  const double patient = footbsim::TempoAdvanceMultiplier(Tempo::PATIENT);
  const double balanced = footbsim::TempoAdvanceMultiplier(Tempo::BALANCED);
  const double direct = footbsim::TempoAdvanceMultiplier(Tempo::DIRECT);

  CHECK(balanced == 1.0);
  CHECK(patient < balanced);
  CHECK(balanced < direct);
}
