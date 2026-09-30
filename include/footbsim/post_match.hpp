#pragma once

#include "footbsim/match_state.hpp"
#include "footbsim/team_stats.hpp"

namespace footbsim
{

  enum class MatchOutcome
  {
    WIN,
    DRAW,
    LOSS
  };

  // Result from one team's own point of view.
  MatchOutcome OutcomeFor(int goalsFor, int goalsAgainst);

  // Moves stats.form and stats.morale toward the target for the given
  // outcome (see the "Post-match stats update" block in tuning.hpp). Every
  // other field is left untouched. The result stays inside form's -10..+10
  // and morale's 0..100 ranges.
  void ApplyMatchOutcome(TeamStats& stats, MatchOutcome outcome);

  // Applies a finished match to both teams' stats: the home side sees
  // result.home_goals as its own and vice versa. Reuse the updated stats
  // for the teams' next match.
  void ApplyMatchResult(TeamStats& home, TeamStats& away, const MatchResult& result);

} // namespace footbsim
