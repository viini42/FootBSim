#include "footbsim/match_engine.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("sigmoid stays within the open unit interval", "[probability]")
{
  // Values far enough out to saturate, but small enough that
  // 1 +/- std::exp(...) stays distinguishable from 1.0 in double
  // precision (which would make the bound exact, not just close).
  CHECK(footbsim::Sigmoid(-30.0) > 0.0);
  CHECK(footbsim::Sigmoid(-30.0) < 0.001);
  CHECK(footbsim::Sigmoid(30.0) < 1.0);
  CHECK(footbsim::Sigmoid(30.0) > 0.999);
  CHECK(footbsim::Sigmoid(0.0) == 0.5);
}

TEST_CASE("duel probability favors the stronger side", "[probability][duel]")
{
  const double even = footbsim::DuelProbability(50.0, 50.0);
  CHECK(even == 0.5);

  const double attacker_favored = footbsim::DuelProbability(80.0, 40.0);
  CHECK(attacker_favored > 0.5);

  const double defender_favored = footbsim::DuelProbability(30.0, 70.0);
  CHECK(defender_favored < 0.5);
}

TEST_CASE("duel probability is monotonic in the stat gap", "[probability][duel]")
{
  const double small_gap = footbsim::DuelProbability(55.0, 50.0);
  const double large_gap = footbsim::DuelProbability(90.0, 50.0);
  CHECK(large_gap > small_gap);
}

TEST_CASE("duel probability never leaves the unit interval", "[probability][duel]")
{
  const double p = footbsim::DuelProbability(500.0, 0.0);
  CHECK(p > 0.0);
  CHECK(p < 1.0);
}
