#include "footbsim/championship.hpp"

#include <catch2/catch_test_macros.hpp>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>

using footbsim::Championship;
using footbsim::ChampionshipOptions;
using footbsim::MatchRecord;
using footbsim::StandingsEntry;
using footbsim::TeamConfig;
using footbsim::TeamStats;

namespace
{

  std::vector<TeamConfig> MakeTeams(int count)
  {
    std::vector<TeamConfig> teams;
    for (int i = 0; i < count; ++i)
    {
      TeamStats stats;
      stats.name = "Team " + std::to_string(i);
      stats.attack = 45.0 + 4.0 * i;
      stats.defense = 45.0 + 4.0 * i;
      stats.midfield = 45.0 + 4.0 * i;
      teams.push_back(TeamConfig{ .stats = stats });
    }
    return teams;
  }

  ChampionshipOptions Seeded(bool doubleRoundRobin = true)
  {
    ChampionshipOptions options;
    options.seed = 7;
    options.double_round_robin = doubleRoundRobin;
    return options;
  }

} // namespace

TEST_CASE("Championship needs at least two teams", "[championship]")
{
  CHECK_THROWS_AS(Championship{ MakeTeams(0) }, std::invalid_argument);
  CHECK_THROWS_AS(Championship{ MakeTeams(1) }, std::invalid_argument);
  CHECK_NOTHROW(Championship{ MakeTeams(2) });
}

TEST_CASE("Round count follows the team count and the round-robin mode", "[championship]")
{
  CHECK(Championship{ MakeTeams(4), Seeded(true) }.RoundCount() == 6);
  CHECK(Championship{ MakeTeams(4), Seeded(false) }.RoundCount() == 3);
  // An odd count adds a bye slot, so 5 teams need 5 rounds per leg.
  CHECK(Championship{ MakeTeams(5), Seeded(false) }.RoundCount() == 5);
}

TEST_CASE("Double round robin: every ordered pair meets exactly once", "[championship]")
{
  Championship championship{ MakeTeams(6), Seeded(true) };
  championship.SimulateSeason();

  std::multiset<std::pair<std::string, std::string>> fixtures;
  for (const MatchRecord& record : championship.Results())
  {
    fixtures.insert({ record.home, record.away });
  }

  CHECK(fixtures.size() == 30);
  for (const TeamConfig& home : championship.Teams())
  {
    for (const TeamConfig& away : championship.Teams())
    {
      if (home.stats.name != away.stats.name)
      {
        CHECK(fixtures.count({ home.stats.name, away.stats.name }) == 1);
      }
    }
  }
}

TEST_CASE("Single round robin with an odd team count: each pair once, one bye per round",
          "[championship]")
{
  Championship championship{ MakeTeams(5), Seeded(false) };

  std::set<std::set<std::string>> pairs;
  while (!championship.IsFinished())
  {
    const std::vector<MatchRecord> round = championship.SimulateNextRound();
    CHECK(round.size() == 2);

    std::set<std::string> busy;
    for (const MatchRecord& record : round)
    {
      busy.insert(record.home);
      busy.insert(record.away);
      pairs.insert({ record.home, record.away });
    }
    CHECK(busy.size() == 4); // nobody plays twice in a round
  }

  CHECK(pairs.size() == 10);
}

TEST_CASE("SimulateNextRound advances one round and is a no-op once finished", "[championship]")
{
  Championship championship{ MakeTeams(4), Seeded(false) };

  CHECK(championship.RoundsPlayed() == 0);
  CHECK(championship.SimulateNextRound().size() == 2);
  CHECK(championship.RoundsPlayed() == 1);

  championship.SimulateSeason();
  CHECK(championship.IsFinished());
  CHECK(championship.SimulateNextRound().empty());
  CHECK(championship.RoundsPlayed() == championship.RoundCount());
}

TEST_CASE("Standings add up from the match results", "[championship]")
{
  Championship championship{ MakeTeams(6), Seeded(true) };
  championship.SimulateSeason();

  std::map<std::string, StandingsEntry> expected;
  for (const MatchRecord& record : championship.Results())
  {
    StandingsEntry& home = expected[record.home];
    StandingsEntry& away = expected[record.away];
    home.goals_for += record.home_goals;
    home.goals_against += record.away_goals;
    away.goals_for += record.away_goals;
    away.goals_against += record.home_goals;
    if (record.home_goals == record.away_goals)
    {
      home.points += 1;
      away.points += 1;
    }
    else
    {
      (record.home_goals > record.away_goals ? home : away).points += 3;
    }
  }

  int total_wins = 0;
  int total_losses = 0;
  for (const StandingsEntry& entry : championship.Standings())
  {
    CHECK(entry.played == 10);
    CHECK(entry.wins + entry.draws + entry.losses == entry.played);
    CHECK(entry.points == entry.wins * 3 + entry.draws);
    CHECK(entry.points == expected[entry.team].points);
    CHECK(entry.goals_for == expected[entry.team].goals_for);
    CHECK(entry.goals_against == expected[entry.team].goals_against);
    total_wins += entry.wins;
    total_losses += entry.losses;
  }
  CHECK(total_wins == total_losses);
}

TEST_CASE("Standings are sorted best first", "[championship]")
{
  Championship championship{ MakeTeams(8), Seeded(true) };
  championship.SimulateSeason();

  const std::vector<StandingsEntry> table = championship.Standings();
  for (std::size_t i = 1; i < table.size(); ++i)
  {
    CHECK(table[i - 1].points >= table[i].points);
  }
}

TEST_CASE("A seeded championship replays identically", "[championship]")
{
  Championship first{ MakeTeams(6), Seeded() };
  Championship second{ MakeTeams(6), Seeded() };
  first.SimulateSeason();
  second.SimulateSeason();

  REQUIRE(first.Results().size() == second.Results().size());
  for (std::size_t i = 0; i < first.Results().size(); ++i)
  {
    CHECK(first.Results()[i].home == second.Results()[i].home);
    CHECK(first.Results()[i].home_goals == second.Results()[i].home_goals);
    CHECK(first.Results()[i].away_goals == second.Results()[i].away_goals);
  }
}

TEST_CASE("Stats update between matches only when enabled", "[championship][post_match]")
{
  ChampionshipOptions off = Seeded();
  off.update_stats_after_match = false;
  Championship frozen{ MakeTeams(4), off };
  frozen.SimulateSeason();
  for (const TeamConfig& team : frozen.Teams())
  {
    CHECK(team.stats.form == 0.0);
    CHECK(team.stats.morale == 50.0);
  }

  Championship live{ MakeTeams(4), Seeded() };
  live.SimulateSeason();
  bool any_changed = false;
  for (const TeamConfig& team : live.Teams())
  {
    any_changed = any_changed || team.stats.form != 0.0 || team.stats.morale != 50.0;
  }
  CHECK(any_changed);
}

TEST_CASE("A much stronger team tops the table", "[championship][smoke]")
{
  std::vector<TeamConfig> teams = MakeTeams(6);
  teams[3].stats.attack = 90.0;
  teams[3].stats.defense = 90.0;
  teams[3].stats.midfield = 90.0;

  Championship championship{ teams, Seeded() };
  championship.SimulateSeason();

  CHECK(championship.Standings().front().team == "Team 3");
}
