#include "footbsim/match_state.hpp"

#include "footbsim/messages.hpp"

#include <iomanip>
#include <iostream>

namespace footbsim
{

  namespace
  {

    void PrintSummaryLine(const std::string& label, int homeValue, int awayValue)
    {
      std::cout << std::left << std::setw(24) << label << std::right << std::setw(8) << homeValue
                << std::setw(10) << awayValue << "\n";
    }

  } // namespace

  void MatchResult::PrintLog(const std::string& homeName, const std::string& awayName) const
  {
    std::cout << homeName << " " << messages::Versus() << " " << awayName << "\n";
    std::cout << "----------------------------------------\n";
    for (const MatchEvent& event : log)
    {
      std::cout << "[" << std::setw(2) << std::setfill('0') << event.minute << std::setfill(' ')
                << "'] " << messages::EventTypeLabel(event.type);
      if (!event.team.empty())
      {
        std::cout << " (" << event.team << ")";
      }
      std::cout << " - " << event.description << "\n";
    }

    std::cout << "----------------------------------------\n";
    std::cout << messages::FinalScore(homeName, home_goals, awayName, away_goals) << "\n\n";

    const int total_possession = home_state.possession_ticks + away_state.possession_ticks;
    const int home_poss_pct =
      total_possession > 0 ? (home_state.possession_ticks * 100) / total_possession : 50;

    std::cout << std::left << std::setw(24) << messages::StatColumnHeader() << std::right
              << std::setw(8) << homeName.substr(0, 8) << std::setw(10) << awayName.substr(0, 8)
              << "\n";
    PrintSummaryLine(messages::PossessionLabel(), home_poss_pct, 100 - home_poss_pct);
    PrintSummaryLine(messages::ShotsLabel(), home_state.shots, away_state.shots);
    PrintSummaryLine(messages::ShotsOnTargetLabel(),
                     home_state.shots_on_target,
                     away_state.shots_on_target);
    PrintSummaryLine(messages::CornersLabel(), home_state.corners, away_state.corners);
    PrintSummaryLine(messages::FoulsLabel(), home_state.fouls, away_state.fouls);
    PrintSummaryLine(messages::YellowCardsLabel(),
                     home_state.yellow_cards,
                     away_state.yellow_cards);
    PrintSummaryLine(messages::RedCardsLabel(), home_state.red_cards, away_state.red_cards);
  }

} // namespace footbsim
