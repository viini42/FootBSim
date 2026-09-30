#include "footbsim/championship.hpp"
#include "footbsim/match_context.hpp"
#include "footbsim/match_engine.hpp"
#include "footbsim/team_config.hpp"
#include "teams_a_b_c.hpp"

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace
{

  std::vector<footbsim::TeamConfig> SerieA2025()
  {
    namespace bra = footbsim::rosters::bra2025;
    const std::vector<footbsim::TeamStats> clubs{
      bra::Atleticomg(),    bra::Bahia(),       bra::Botafogo(),  bra::Bragantino(),
      bra::CearaSC(),       bra::Corinthians(), bra::Cruzeiro(),  bra::ECVitoria(),
      bra::Flamengo(),      bra::Fluminense(),  bra::Fortaleza(), bra::Gremio(),
      bra::Internacional(), bra::Juventude(),   bra::Mirassol(),  bra::Palmeiras(),
      bra::Santos(),        bra::SportRecife(), bra::SaoPaulo(),  bra::VascoDaGama(),
    };

    std::vector<footbsim::TeamConfig> teams;
    for (const footbsim::TeamStats& club : clubs)
    {
      teams.push_back(footbsim::TeamConfig{ .stats = club });
    }
    return teams;
  }

  void RunSeason(std::optional<std::uint64_t> seed)
  {
    footbsim::ChampionshipOptions options;
    options.seed = seed;

    footbsim::Championship championship{ SerieA2025(), options };
    championship.SimulateSeason();
    footbsim::PrintStandings(championship.Standings());
  }

  void RunSingleMatch(std::optional<std::uint64_t> seed)
  {
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
  }

} // namespace

// Usage: footbsim_cli [season] [seed]
//   (no mode)  simulate one match with the full play-by-play log
//   season     simulate a whole Série A 2025 season and print the table
int main(int argc, char** argv)
{
  int next_arg = 1;
  const bool season = argc > next_arg && std::string{ argv[next_arg] } == "season";
  if (season)
  {
    ++next_arg;
  }

  std::optional<std::uint64_t> seed;
  if (argc > next_arg)
  {
    seed = static_cast<std::uint64_t>(std::stoull(argv[next_arg]));
  }

  if (season)
  {
    RunSeason(seed);
  }
  else
  {
    RunSingleMatch(seed);
  }

  return 0;
}
