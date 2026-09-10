#pragma once

#include "footbsim/events.hpp"

#include <string>
#include <vector>

namespace footbsim
{

  struct TeamMatchState
  {
    int shots = 0;
    int shots_on_target = 0;
    int goals = 0;
    int corners = 0;
    int fouls = 0;
    int yellow_cards = 0;
    int red_cards = 0;
    bool has_red_card = false;
    int possession_ticks = 0;
  };

  struct MatchResult
  {
    int home_goals = 0;
    int away_goals = 0;
    TeamMatchState home_state;
    TeamMatchState away_state;
    std::vector<MatchEvent> log;

    // Prints the full event log followed by a summary stat table to stdout.
    void PrintLog(const std::string& homeName, const std::string& awayName) const;
  };

} // namespace footbsim
