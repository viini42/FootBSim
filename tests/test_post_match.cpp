#include "footbsim/post_match.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using Catch::Approx;
using footbsim::MatchOutcome;
using footbsim::MatchResult;
using footbsim::TeamStats;

TEST_CASE("OutcomeFor reads the score from the team's own point of view", "[post_match]")
{
  CHECK(footbsim::OutcomeFor(2, 1) == MatchOutcome::WIN);
  CHECK(footbsim::OutcomeFor(1, 1) == MatchOutcome::DRAW);
  CHECK(footbsim::OutcomeFor(0, 3) == MatchOutcome::LOSS);
}

TEST_CASE("A win raises form and morale, a loss lowers them, a draw pulls toward neutral",
          "[post_match]")
{
  TeamStats won;
  footbsim::ApplyMatchOutcome(won, MatchOutcome::WIN);
  CHECK(won.form > 0.0);
  CHECK(won.morale > 50.0);

  TeamStats lost;
  footbsim::ApplyMatchOutcome(lost, MatchOutcome::LOSS);
  CHECK(lost.form < 0.0);
  CHECK(lost.morale < 50.0);

  TeamStats hot;
  hot.form = 8.0;
  hot.morale = 90.0;
  footbsim::ApplyMatchOutcome(hot, MatchOutcome::DRAW);
  CHECK(hot.form < 8.0);
  CHECK(hot.form > 0.0);
  CHECK(hot.morale < 90.0);
  CHECK(hot.morale > 50.0);
}

TEST_CASE("Only form and morale change", "[post_match]")
{
  TeamStats stats;
  stats.name = "Team";
  stats.attack = 61.0;
  stats.defense = 62.0;
  stats.midfield = 63.0;
  stats.stamina = 84.0;
  stats.discipline = 55.0;
  stats.aggression = 45.0;

  footbsim::ApplyMatchOutcome(stats, MatchOutcome::WIN);

  CHECK(stats.name == "Team");
  CHECK(stats.attack == Approx(61.0));
  CHECK(stats.defense == Approx(62.0));
  CHECK(stats.midfield == Approx(63.0));
  CHECK(stats.stamina == Approx(84.0));
  CHECK(stats.discipline == Approx(55.0));
  CHECK(stats.aggression == Approx(45.0));
}

TEST_CASE("Streaks converge inside the documented ranges without overshooting", "[post_match]")
{
  TeamStats winning;
  TeamStats losing;
  for (int i = 0; i < 200; ++i)
  {
    footbsim::ApplyMatchOutcome(winning, MatchOutcome::WIN);
    footbsim::ApplyMatchOutcome(losing, MatchOutcome::LOSS);
  }

  CHECK(winning.form <= 10.0);
  CHECK(winning.form > 9.9);
  CHECK(losing.form >= -10.0);
  CHECK(losing.form < -9.9);
  CHECK(winning.morale <= 100.0);
  CHECK(losing.morale >= 0.0);
}

TEST_CASE("ApplyMatchResult credits each side with its own result", "[post_match]")
{
  TeamStats home;
  TeamStats away;
  MatchResult result;
  result.home_goals = 3;
  result.away_goals = 1;

  footbsim::ApplyMatchResult(home, away, result);

  CHECK(home.form > 0.0);
  CHECK(away.form < 0.0);
  CHECK(home.morale > away.morale);
}
