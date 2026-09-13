#include "footbsim/match_context.hpp"
#include "footbsim/match_engine.hpp"
#include "footbsim/team_stats.hpp"

#include <catch2/catch_test_macros.hpp>

using footbsim::MatchContext;
using footbsim::MatchEngine;
using footbsim::MatchResult;
using footbsim::TeamStats;

namespace
{

  TeamStats MakeTeam(std::string name)
  {
    TeamStats s;
    s.name = std::move(name);
    s.attack = 65;
    s.defense = 60;
    s.midfield = 62;
    s.form = 1;
    s.stamina = 85;
    s.discipline = 55;
    s.morale = 65;
    return s;
  }

} // namespace

TEST_CASE("seeded match completes and produces a sane scoreline", "[engine][smoke]")
{
  const TeamStats home = MakeTeam("Home FC");
  const TeamStats away = MakeTeam("Away FC");
  MatchContext context(home, away);

  MatchEngine engine(home, away, context, /*seed=*/42);
  const MatchResult result = engine.Simulate();

  CHECK_FALSE(result.log.empty());
  CHECK(result.log.front().type == footbsim::EventType::KICKOFF);
  CHECK(result.log.back().type == footbsim::EventType::FULL_TIME);

  CHECK(result.home_goals >= 0);
  CHECK(result.away_goals >= 0);
  CHECK(result.home_goals < 15); // sanity ceiling, not a real rule
  CHECK(result.away_goals < 15);

  CHECK(result.home_goals == result.home_state.goals);
  CHECK(result.away_goals == result.away_state.goals);
}

TEST_CASE("same seed produces identical results", "[engine][smoke]")
{
  const TeamStats home = MakeTeam("Home FC");
  const TeamStats away = MakeTeam("Away FC");
  MatchContext context(home, away);

  MatchEngine engine_a(home, away, context, /*seed=*/1234);
  MatchEngine engine_b(home, away, context, /*seed=*/1234);

  const MatchResult a = engine_a.Simulate();
  const MatchResult b = engine_b.Simulate();

  CHECK(a.home_goals == b.home_goals);
  CHECK(a.away_goals == b.away_goals);
  CHECK(a.log.size() == b.log.size());
}

TEST_CASE("much stronger team wins more often across several seeds", "[engine][smoke]")
{
  TeamStats strong = MakeTeam("Strong FC");
  strong.attack = 90;
  strong.defense = 85;
  strong.midfield = 88;

  TeamStats weak = MakeTeam("Weak FC");
  weak.attack = 35;
  weak.defense = 30;
  weak.midfield = 32;

  MatchContext context(strong, weak);

  int strong_wins = 0;
  constexpr int TRIALS = 25;
  for (std::uint64_t seed = 0; seed < TRIALS; ++seed)
  {
    MatchEngine engine(strong, weak, context, seed);
    const MatchResult result = engine.Simulate();
    if (result.home_goals > result.away_goals)
    {
      ++strong_wins;
    }
  }

  CHECK(strong_wins >= TRIALS * 0.7); // should dominate, not win every single time
}

TEST_CASE("more aggressive team takes more shots on average", "[engine][smoke][aggression]")
{
  TeamStats attacking = MakeTeam("Attacking FC");
  attacking.aggression = 90;

  TeamStats defensive = MakeTeam("Defensive FC");
  defensive.aggression = 10;

  MatchContext context(attacking, defensive);

  int attacking_shots = 0;
  int defensive_shots = 0;
  constexpr int TRIALS = 25;
  for (std::uint64_t seed = 0; seed < TRIALS; ++seed)
  {
    MatchEngine engine(attacking, defensive, context, seed);
    const MatchResult result = engine.Simulate();
    attacking_shots += result.home_state.shots;
    defensive_shots += result.away_state.shots;
  }

  CHECK(attacking_shots > defensive_shots);
}
