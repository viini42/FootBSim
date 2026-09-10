#include "footbsim/match_engine.hpp"
#include "micro_test.hpp"

FOOTBSIM_TEST(sigmoid_stays_within_open_unit_interval)
{
  // Values far enough out to saturate, but small enough that
  // 1 +/- std::exp(...) stays distinguishable from 1.0 in double
  // precision (which would make the bound exact, not just close).
  FOOTBSIM_CHECK(footbsim::Sigmoid(-30.0) > 0.0);
  FOOTBSIM_CHECK(footbsim::Sigmoid(-30.0) < 0.001);
  FOOTBSIM_CHECK(footbsim::Sigmoid(30.0) < 1.0);
  FOOTBSIM_CHECK(footbsim::Sigmoid(30.0) > 0.999);
  FOOTBSIM_CHECK(footbsim::Sigmoid(0.0) == 0.5);
}

FOOTBSIM_TEST(duel_probability_favors_the_stronger_side)
{
  const double even = footbsim::DuelProbability(50.0, 50.0);
  FOOTBSIM_CHECK(even == 0.5);

  const double attacker_favored = footbsim::DuelProbability(80.0, 40.0);
  FOOTBSIM_CHECK(attacker_favored > 0.5);

  const double defender_favored = footbsim::DuelProbability(30.0, 70.0);
  FOOTBSIM_CHECK(defender_favored < 0.5);
}

FOOTBSIM_TEST(duel_probability_is_monotonic_in_the_stat_gap)
{
  const double small_gap = footbsim::DuelProbability(55.0, 50.0);
  const double large_gap = footbsim::DuelProbability(90.0, 50.0);
  FOOTBSIM_CHECK(large_gap > small_gap);
}

FOOTBSIM_TEST(duel_probability_never_leaves_the_unit_interval)
{
  const double p = footbsim::DuelProbability(500.0, 0.0);
  FOOTBSIM_CHECK(p > 0.0);
  FOOTBSIM_CHECK(p < 1.0);
}
