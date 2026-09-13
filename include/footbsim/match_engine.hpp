#pragma once

#include "footbsim/match_context.hpp"
#include "footbsim/match_state.hpp"
#include "footbsim/pitch_zone.hpp"
#include "footbsim/rng.hpp"
#include "footbsim/team_config.hpp"
#include "footbsim/team_stats.hpp"
#include "footbsim/tuning.hpp"

#include <cstdint>
#include <optional>

namespace footbsim
{

  // ---- Pure statistical primitives -----------------------------------------
  // Kept free of engine/RNG state so they're trivially unit-testable.

  // Effective attack for one minute of play: base stat scaled by recent form,
  // stamina, home advantage (1.0 if not applicable), weather accuracy and
  // tactical aggression, plus a morale-driven noise term (can be negative).
  double EffectiveAttack(const TeamStats& stats,
                         double staminaFactor,
                         double homeFactor,
                         double weatherFactor,
                         double aggressionFactor,
                         double moraleNoise);

  // Effective defense for one minute of play. No weather penalty (accuracy
  // doesn't apply to defending) and homeFactor should be 1.0 unless you
  // deliberately want a home boost to defense too.
  double EffectiveDefense(const TeamStats& stats,
                          double staminaFactor,
                          double homeFactor,
                          double aggressionFactor,
                          double moraleNoise);

  // Multiplier in (0, 1] representing fatigue at the given minute (1-90+).
  // Teams with lower base stamina decay faster and further.
  double StaminaDecayFactor(double baseStamina, int minute);

  // Tactical aggression trade-off: a more attacking team (aggression -> 100)
  // hits harder going forward but leaves more gaps at the back; a more
  // defensive team (aggression -> 0) does the reverse. Neutral at 50.
  double AggressionAttackFactor(double aggression);
  double AggressionDefenseFactor(double aggression);

  // How eagerly a team commits to the ball: raises a possessing team's shot
  // eagerness and a defending team's foul rate when they play aggressively.
  double AggressionPressureFactor(double aggression);

  // TeamStats::aggression shifted by the team's chosen Formation for this
  // match (see Formation's doc comment), clamped back to [0,100] before
  // feeding into AggressionAttackFactor/DefenseFactor/PressureFactor.
  double EffectiveAggression(const TeamConfig& team);

  double Sigmoid(double x);

  // Probability that "attacker" wins a duel against "defender" -- used for
  // zone advancement, shot outcomes, and first-possession rolls alike.
  // `bias` shifts the baseline for two evenly-matched sides away from a
  // 50/50 coin flip (e.g. possession retention, where real passing success
  // is well above chance); leave at 0 for duels that should be a true
  // coin flip between equal sides (kickoff, goal conversion).
  double DuelProbability(double effectiveAttacker,
                         double effectiveDefender,
                         double steepness = tuning::ZONE_DUEL_STEEPNESS,
                         double bias = 0.0);

  // ---- Engine ---------------------------------------------------------------

  class MatchEngine
  {
  public:
    MatchEngine(TeamConfig home,
                TeamConfig away,
                MatchContext context,
                std::optional<std::uint64_t> seed = std::nullopt);

    MatchResult Simulate();

  private:
    struct MinuteEffectiveStats
    {
      double possessing_attack = 0.0;
      double possessing_midfield = 0.0;
      double defending_defense = 0.0;
      double defending_midfield = 0.0;
    };

    bool DecideFirstPossession();
    MinuteEffectiveStats ComputeMinuteStats(bool homeHasBall, int minute);
    void MaybeGenerateFoul(bool homeIsDefending, int minute, MatchResult& result);
    void ResolveMidfieldZone(bool& homeHasBall,
                             PitchZone& zone,
                             const MinuteEffectiveStats& eff,
                             int minute,
                             MatchResult& result);
    void ResolveAttackingZone(bool& homeHasBall,
                              PitchZone& zone,
                              const MinuteEffectiveStats& eff,
                              int minute,
                              MatchResult& result);

    TeamConfig m_home;
    TeamConfig m_away;
    MatchContext m_context;
    Rng m_rng;
  };

} // namespace footbsim
