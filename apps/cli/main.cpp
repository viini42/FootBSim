#include "footbsim/match_context.hpp"
#include "footbsim/match_engine.hpp"
#include "footbsim/team_stats.hpp"
#include "brasileirao_2025.hpp"

#include <cstdint>
#include <optional>

int main(int argc, char** argv)
{
  std::optional<std::uint64_t> seed = 0;
  if (argc > 1)
  {
    seed = static_cast<std::uint64_t>(std::stoull(argv[1]));
  }

  const footbsim::TeamStats home = footbsim::rosters::bra2025::Flamengo();
  const footbsim::TeamStats away = footbsim::rosters::bra2025::Palmeiras();

  footbsim::MatchContext context;
  context.home_advantage = footbsim::HomeAdvantageLevel::SLIGHT;
  context.weather = footbsim::Weather::CLEAR;
  context.referee_strictness = footbsim::RefereeStyle::BALANCED;
  context.stakes = footbsim::Stakes::NORMAL;
  context.travel_fatigue = footbsim::TravelFatigueLevel::DIFFERENT_STATE;

  footbsim::MatchEngine engine(home, away, context, seed);
  const footbsim::MatchResult result = engine.Simulate();

  result.PrintLog(home.name, away.name);

  return 0;
}
