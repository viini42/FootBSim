#include "footbsim/match_context.hpp"
#include "footbsim/match_engine.hpp"
#include "footbsim/team_config.hpp"
#include "teams_a_b_c.hpp"

#include <cstdint>
#include <optional>

int main(int argc, char** argv)
{
  std::optional<std::uint64_t> seed;
  if (argc > 1)
  {
    seed = static_cast<std::uint64_t>(std::stoull(argv[1]));
  }

  footbsim::TeamConfig home{ .stats = footbsim::rosters::bra2025::Mirassol() };
  home.formation = footbsim::Formation::FOUR_THREE_THREE;
  home.press_intensity = footbsim::PressIntensity::HIGH_PRESS;
  home.tempo = footbsim::Tempo::DIRECT;

  footbsim::TeamConfig away{ .stats = footbsim::rosters::bra2025::Coritiba() };
  away.formation = footbsim::Formation::FIVE_THREE_TWO;
  away.press_intensity = footbsim::PressIntensity::LOW_BLOCK;
  away.tempo = footbsim::Tempo::PATIENT;

  footbsim::MatchContext context{ home.stats, away.stats };
  context.home_advantage = footbsim::HomeAdvantageLevel::SLIGHT;
  context.weather = footbsim::Weather::CLEAR;
  context.referee_strictness = footbsim::RefereeStyle::BALANCED;
  context.stakes = footbsim::Stakes::NORMAL;

  footbsim::MatchEngine engine{ home, away, context, seed };
  const footbsim::MatchResult result = engine.Simulate();

  result.PrintLog(home.stats.name, away.stats.name);

  return 0;
}
