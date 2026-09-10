#include "footbsim/match_context.hpp"
#include "footbsim/match_engine.hpp"
#include "footbsim/team_stats.hpp"

#include <cstdint>
#include <optional>

namespace
{

  footbsim::TeamStats SampleHome()
  {
    footbsim::TeamStats s;
    s.name = "Riverside United";
    s.attack = 78;
    s.defense = 65;
    s.midfield = 72;
    s.form = 4;
    s.stamina = 88;
    s.discipline = 60;
    s.morale = 75;
    s.aggression = 68;
    return s;
  }

  footbsim::TeamStats SampleAway()
  {
    footbsim::TeamStats s;
    s.name = "Ashford City";
    s.attack = 62;
    s.defense = 70;
    s.midfield = 58;
    s.form = -2;
    s.stamina = 80;
    s.discipline = 45;
    s.morale = 55;
    s.aggression = 32;
    return s;
  }

} // namespace

int main(int argc, char** argv)
{
  std::optional<std::uint64_t> seed;
  if (argc > 1)
  {
    seed = static_cast<std::uint64_t>(std::stoull(argv[1]));
  }

  const footbsim::TeamStats home = SampleHome();
  const footbsim::TeamStats away = SampleAway();

  footbsim::MatchContext context;
  context.home_advantage = footbsim::HomeAdvantageLevel::STRONG;
  context.weather = footbsim::Weather::CLEAR;
  context.referee_strictness = footbsim::RefereeStyle::STRICT;
  context.stakes = footbsim::Stakes::NORMAL;
  context.travel_fatigue = footbsim::TravelFatigueLevel::DIFFERENT_STATE;

  footbsim::MatchEngine engine(home, away, context, seed);
  const footbsim::MatchResult result = engine.Simulate();

  result.PrintLog(home.name, away.name);

  return 0;
}
