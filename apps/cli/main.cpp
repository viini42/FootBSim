#include "footbsim/match_context.hpp"
#include "footbsim/match_engine.hpp"
#include "footbsim/team_stats.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
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

  void PrintSummaryLine(const std::string& label, int homeValue, int awayValue)
  {
    std::cout << std::left << std::setw(20) << label << std::right << std::setw(8) << homeValue
              << std::setw(10) << awayValue << "\n";
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
  context.travel_fatigue = 5.0;

  footbsim::MatchEngine engine(home, away, context, seed);
  const footbsim::MatchResult result = engine.Simulate();

  std::cout << home.name << " vs " << away.name << "\n";
  std::cout << "----------------------------------------\n";
  for (const footbsim::MatchEvent& event : result.log)
  {
    std::cout << "[" << std::setw(2) << std::setfill('0') << event.minute << std::setfill(' ')
              << "'] " << ToString(event.type);
    if (!event.team.empty())
    {
      std::cout << " (" << event.team << ")";
    }
    std::cout << " - " << event.description << "\n";
  }

  std::cout << "----------------------------------------\n";
  std::cout << "FULL TIME: " << home.name << " " << result.home_goals << " - " << result.away_goals
            << " " << away.name << "\n\n";

  const int total_possession =
    result.home_state.possession_ticks + result.away_state.possession_ticks;
  const int home_poss_pct =
    total_possession > 0 ? (result.home_state.possession_ticks * 100) / total_possession : 50;

  std::cout << std::left << std::setw(20) << "Stat" << std::right << std::setw(8)
            << home.name.substr(0, 8) << std::setw(10) << away.name.substr(0, 8) << "\n";
  PrintSummaryLine("Possession %", home_poss_pct, 100 - home_poss_pct);
  PrintSummaryLine("Shots", result.home_state.shots, result.away_state.shots);
  PrintSummaryLine("Shots on target",
                   result.home_state.shots_on_target,
                   result.away_state.shots_on_target);
  PrintSummaryLine("Corners", result.home_state.corners, result.away_state.corners);
  PrintSummaryLine("Fouls", result.home_state.fouls, result.away_state.fouls);
  PrintSummaryLine("Yellow cards", result.home_state.yellow_cards, result.away_state.yellow_cards);
  PrintSummaryLine("Red cards", result.home_state.red_cards, result.away_state.red_cards);

  return 0;
}
