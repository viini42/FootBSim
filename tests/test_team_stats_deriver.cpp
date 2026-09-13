#include "footbsim/utils/team_stats_deriver.hpp"

#include <catch2/catch_test_macros.hpp>

using footbsim::TeamStats;
using footbsim::utils::DeriveTeamStats;
using footbsim::utils::TeamRawStats;

namespace
{

  TeamRawStats MakeRaw(std::string name)
  {
    TeamRawStats t;
    t.name = std::move(name);
    t.matches = 38;
    t.wins = 15;
    t.draws = 10;
    t.losses = 13;
    t.goals_for = 50;
    t.goals_against = 45;
    t.shots = 400;
    t.shots_on_target = 140;
    t.shots_against = 400;
    t.shots_on_target_against = 140;
    t.possession_pct = 50.0;
    t.pass_accuracy_pct = 85.0;
    t.fouls = 400;
    t.corners = 180;
    t.yellow_cards = 70;
    t.red_cards = 3;
    return t;
  }

} // namespace

TEST_CASE("a single team on its own comes back exactly average", "[team_stats_deriver]")
{
  const std::vector<TeamRawStats> teams = { MakeRaw("Solo FC") };
  const std::vector<TeamStats> derived = DeriveTeamStats(teams);

  REQUIRE(derived.size() == 1);
  CHECK(derived[0].name == "Solo FC");
  CHECK(derived[0].attack == 50.0);
  CHECK(derived[0].defense == 50.0);
  CHECK(derived[0].midfield == 50.0);
  CHECK(derived[0].discipline == 50.0);
  CHECK(derived[0].aggression == 50.0);
}

TEST_CASE("a team with better attacking output and results scores higher on attack",
          "[team_stats_deriver]")
{
  TeamRawStats strong = MakeRaw("Strong FC");
  strong.shots_on_target = 220;
  strong.goals_for = 80;
  strong.wins = 28;
  strong.draws = 5;
  strong.losses = 5;

  TeamRawStats weak = MakeRaw("Weak FC");
  weak.shots_on_target = 90;
  weak.goals_for = 30;
  weak.wins = 5;
  weak.draws = 5;
  weak.losses = 28;

  const std::vector<TeamStats> derived = DeriveTeamStats({ strong, weak });

  REQUIRE(derived.size() == 2);
  CHECK(derived[0].attack > derived[1].attack);
}

TEST_CASE("a team that concedes far more shots on target scores lower on defense",
          "[team_stats_deriver]")
{
  TeamRawStats solid = MakeRaw("Solid FC");
  solid.shots_on_target_against = 80;
  solid.goals_against = 20;

  TeamRawStats leaky = MakeRaw("Leaky FC");
  leaky.shots_on_target_against = 220;
  leaky.goals_against = 70;

  const std::vector<TeamStats> derived = DeriveTeamStats({ solid, leaky });

  REQUIRE(derived.size() == 2);
  CHECK(derived[0].defense > derived[1].defense);
}

TEST_CASE("a team with more cards and fouls per game scores lower on discipline",
          "[team_stats_deriver]")
{
  TeamRawStats clean = MakeRaw("Clean FC");
  clean.yellow_cards = 40;
  clean.red_cards = 1;
  clean.fouls = 300;

  TeamRawStats dirty = MakeRaw("Dirty FC");
  dirty.yellow_cards = 100;
  dirty.red_cards = 8;
  dirty.fouls = 550;

  const std::vector<TeamStats> derived = DeriveTeamStats({ clean, dirty });

  REQUIRE(derived.size() == 2);
  CHECK(derived[0].discipline > derived[1].discipline);
}

TEST_CASE("derived stats stay within the [5,95] band even for extreme inputs",
          "[team_stats_deriver]")
{
  TeamRawStats dominant = MakeRaw("Dominant FC");
  dominant.shots_on_target = 400;
  dominant.goals_for = 150;
  dominant.wins = 38;
  dominant.draws = 0;
  dominant.losses = 0;
  dominant.shots_on_target_against = 5;
  dominant.goals_against = 2;

  TeamRawStats hopeless = MakeRaw("Hopeless FC");
  hopeless.shots_on_target = 10;
  hopeless.goals_for = 5;
  hopeless.wins = 0;
  hopeless.draws = 0;
  hopeless.losses = 38;
  hopeless.shots_on_target_against = 400;
  hopeless.goals_against = 150;

  const std::vector<TeamStats> derived = DeriveTeamStats({ dominant, hopeless });

  for (const TeamStats& s : derived)
  {
    CHECK(s.attack >= 5.0);
    CHECK(s.attack <= 95.0);
    CHECK(s.defense >= 5.0);
    CHECK(s.defense <= 95.0);
  }
}

TEST_CASE("league_strength_offset shifts attack/defense/midfield but not discipline/aggression",
          "[team_stats_deriver]")
{
  const std::vector<TeamRawStats> teams = { MakeRaw("Team A"), MakeRaw("Team B") };

  const std::vector<TeamStats> baseline = DeriveTeamStats(teams);
  const std::vector<TeamStats> shifted = DeriveTeamStats(teams, -8.0);

  REQUIRE(shifted.size() == baseline.size());
  for (std::size_t i = 0; i < baseline.size(); ++i)
  {
    CHECK(shifted[i].attack == baseline[i].attack - 8.0);
    CHECK(shifted[i].defense == baseline[i].defense - 8.0);
    CHECK(shifted[i].midfield == baseline[i].midfield - 8.0);
    CHECK(shifted[i].discipline == baseline[i].discipline);
    CHECK(shifted[i].aggression == baseline[i].aggression);
  }
}

TEST_CASE("league_strength_offset stays clamped to [5,95] even at the extremes",
          "[team_stats_deriver]")
{
  const std::vector<TeamRawStats> teams = { MakeRaw("Team A"), MakeRaw("Team B") };

  const std::vector<TeamStats> pushed_down = DeriveTeamStats(teams, -100.0);
  const std::vector<TeamStats> pushed_up = DeriveTeamStats(teams, 100.0);

  for (const TeamStats& s : pushed_down)
  {
    CHECK(s.attack == 5.0);
    CHECK(s.defense == 5.0);
    CHECK(s.midfield == 5.0);
  }
  for (const TeamStats& s : pushed_up)
  {
    CHECK(s.attack == 95.0);
    CHECK(s.defense == 95.0);
    CHECK(s.midfield == 95.0);
  }
}

TEST_CASE("form, stamina and morale are left at neutral defaults", "[team_stats_deriver]")
{
  const std::vector<TeamStats> derived = DeriveTeamStats({ MakeRaw("Team A"), MakeRaw("Team B") });

  for (const TeamStats& s : derived)
  {
    CHECK(s.form == 0.0);
    CHECK(s.stamina == 100.0);
    CHECK(s.morale == 50.0);
  }
}
