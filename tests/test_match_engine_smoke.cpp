#include "footbsim/match_context.hpp"
#include "footbsim/match_engine.hpp"
#include "footbsim/team_stats.hpp"
#include "micro_test.hpp"

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

FOOTBSIM_TEST(seeded_match_completes_and_produces_a_sane_scoreline)
{
  const TeamStats home = MakeTeam("Home FC");
  const TeamStats away = MakeTeam("Away FC");
  MatchContext context;

  MatchEngine engine(home, away, context, /*seed=*/42);
  const MatchResult result = engine.Simulate();

  FOOTBSIM_CHECK(!result.log.empty());
  FOOTBSIM_CHECK(result.log.front().type == footbsim::EventType::KICKOFF);
  FOOTBSIM_CHECK(result.log.back().type == footbsim::EventType::FULL_TIME);

  FOOTBSIM_CHECK(result.home_goals >= 0);
  FOOTBSIM_CHECK(result.away_goals >= 0);
  FOOTBSIM_CHECK(result.home_goals < 15); // sanity ceiling, not a real rule
  FOOTBSIM_CHECK(result.away_goals < 15);

  FOOTBSIM_CHECK(result.home_goals == result.home_state.goals);
  FOOTBSIM_CHECK(result.away_goals == result.away_state.goals);
}

FOOTBSIM_TEST(same_seed_produces_identical_results)
{
  const TeamStats home = MakeTeam("Home FC");
  const TeamStats away = MakeTeam("Away FC");
  MatchContext context;

  MatchEngine engine_a(home, away, context, /*seed=*/1234);
  MatchEngine engine_b(home, away, context, /*seed=*/1234);

  const MatchResult a = engine_a.Simulate();
  const MatchResult b = engine_b.Simulate();

  FOOTBSIM_CHECK(a.home_goals == b.home_goals);
  FOOTBSIM_CHECK(a.away_goals == b.away_goals);
  FOOTBSIM_CHECK(a.log.size() == b.log.size());
}

FOOTBSIM_TEST(much_stronger_team_wins_more_often_across_several_seeds)
{
  TeamStats strong = MakeTeam("Strong FC");
  strong.attack = 90;
  strong.defense = 85;
  strong.midfield = 88;

  TeamStats weak = MakeTeam("Weak FC");
  weak.attack = 35;
  weak.defense = 30;
  weak.midfield = 32;

  MatchContext context;

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

  FOOTBSIM_CHECK(strong_wins >= TRIALS * 0.7); // should dominate, not win every single time
}

FOOTBSIM_TEST(more_aggressive_team_takes_more_shots_on_average)
{
  TeamStats attacking = MakeTeam("Attacking FC");
  attacking.aggression = 90;

  TeamStats defensive = MakeTeam("Defensive FC");
  defensive.aggression = 10;

  MatchContext context;

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

  FOOTBSIM_CHECK(attacking_shots > defensive_shots);
}
