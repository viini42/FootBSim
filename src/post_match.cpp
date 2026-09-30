#include "footbsim/post_match.hpp"

#include "footbsim/tuning.hpp"

#include <algorithm>

namespace footbsim
{

  namespace
  {

    double FormTarget(MatchOutcome outcome)
    {
      switch (outcome)
      {
      case MatchOutcome::WIN:
        return tuning::FORM_TARGET_WIN;
      case MatchOutcome::DRAW:
        return tuning::FORM_TARGET_DRAW;
      case MatchOutcome::LOSS:
        return tuning::FORM_TARGET_LOSS;
      }
      return tuning::FORM_TARGET_DRAW;
    }

    double MoraleTarget(MatchOutcome outcome)
    {
      switch (outcome)
      {
      case MatchOutcome::WIN:
        return tuning::MORALE_TARGET_WIN;
      case MatchOutcome::DRAW:
        return tuning::MORALE_TARGET_DRAW;
      case MatchOutcome::LOSS:
        return tuning::MORALE_TARGET_LOSS;
      }
      return tuning::MORALE_TARGET_DRAW;
    }

    double MoveToward(double current, double target, double rate)
    {
      return current + (target - current) * rate;
    }

  } // namespace

  MatchOutcome OutcomeFor(int goalsFor, int goalsAgainst)
  {
    if (goalsFor > goalsAgainst)
    {
      return MatchOutcome::WIN;
    }
    return goalsFor == goalsAgainst ? MatchOutcome::DRAW : MatchOutcome::LOSS;
  }

  void ApplyMatchOutcome(TeamStats& stats, MatchOutcome outcome)
  {
    stats.form = std::clamp(MoveToward(stats.form, FormTarget(outcome), tuning::FORM_UPDATE_RATE),
                            tuning::FORM_TARGET_LOSS,
                            tuning::FORM_TARGET_WIN);
    stats.morale =
      std::clamp(MoveToward(stats.morale, MoraleTarget(outcome), tuning::MORALE_UPDATE_RATE),
                 0.0,
                 100.0);
  }

  void ApplyMatchResult(TeamStats& home, TeamStats& away, const MatchResult& result)
  {
    ApplyMatchOutcome(home, OutcomeFor(result.home_goals, result.away_goals));
    ApplyMatchOutcome(away, OutcomeFor(result.away_goals, result.home_goals));
  }

} // namespace footbsim
