#include "footbsim/post_match.hpp"

#include "footbsim/tuning.hpp"

#include <algorithm>

namespace
{

  double FormTarget(footbsim::MatchOutcome outcome)
  {
    switch (outcome)
    {
    case footbsim::MatchOutcome::WIN:
      return footbsim::tuning::FORM_TARGET_WIN;
    case footbsim::MatchOutcome::DRAW:
      return footbsim::tuning::FORM_TARGET_DRAW;
    case footbsim::MatchOutcome::LOSS:
      return footbsim::tuning::FORM_TARGET_LOSS;
    }
    return footbsim::tuning::FORM_TARGET_DRAW;
  }

  double MoraleTarget(footbsim::MatchOutcome outcome)
  {
    switch (outcome)
    {
    case footbsim::MatchOutcome::WIN:
      return footbsim::tuning::MORALE_TARGET_WIN;
    case footbsim::MatchOutcome::DRAW:
      return footbsim::tuning::MORALE_TARGET_DRAW;
    case footbsim::MatchOutcome::LOSS:
      return footbsim::tuning::MORALE_TARGET_LOSS;
    }
    return footbsim::tuning::MORALE_TARGET_DRAW;
  }

  double MoveToward(double current, double target, double rate)
  {
    return current + (target - current) * rate;
  }

} // namespace

footbsim::MatchOutcome footbsim::OutcomeFor(int goalsFor, int goalsAgainst)
{
  if (goalsFor > goalsAgainst)
  {
    return MatchOutcome::WIN;
  }
  return goalsFor == goalsAgainst ? MatchOutcome::DRAW : MatchOutcome::LOSS;
}

void footbsim::ApplyMatchOutcome(TeamStats& stats, MatchOutcome outcome)
{
  stats.form = std::clamp(MoveToward(stats.form, FormTarget(outcome), tuning::FORM_UPDATE_RATE),
                          tuning::FORM_TARGET_LOSS,
                          tuning::FORM_TARGET_WIN);
  stats.morale =
    std::clamp(MoveToward(stats.morale, MoraleTarget(outcome), tuning::MORALE_UPDATE_RATE),
               0.0,
               100.0);
}

void footbsim::ApplyMatchResult(TeamStats& home, TeamStats& away, const MatchResult& result)
{
  ApplyMatchOutcome(home, OutcomeFor(result.home_goals, result.away_goals));
  ApplyMatchOutcome(away, OutcomeFor(result.away_goals, result.home_goals));
}
