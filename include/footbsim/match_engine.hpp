#pragma once

#include "footbsim/match_context.hpp"
#include "footbsim/match_state.hpp"
#include "footbsim/pitch_zone.hpp"
#include "footbsim/rng.hpp"
#include "footbsim/team_stats.hpp"

#include <cstdint>
#include <optional>

namespace footbsim
{

  // ---- Pure statistical primitives -----------------------------------------
  // Kept free of engine/RNG state so they're trivially unit-testable.

  // Effective attack for one minute of play: base stat scaled by recent form,
  // stamina, home advantage (1.0 if not applicable) and weather accuracy, plus
  // a morale-driven noise term (can be negative).
  double EffectiveAttack(const TeamStats& stats,
                         double staminaFactor,
                         double homeFactor,
                         double weatherFactor,
                         double moraleNoise);

  // Effective defense for one minute of play. No weather penalty (accuracy
  // doesn't apply to defending) and homeFactor should be 1.0 unless you
  // deliberately want a home boost to defense too.
  double EffectiveDefense(const TeamStats& stats,
                          double staminaFactor,
                          double homeFactor,
                          double moraleNoise);

  // Multiplier in (0, 1] representing fatigue at the given minute (1-90+).
  // Teams with lower base stamina decay faster and further.
  double StaminaDecayFactor(double baseStamina, int minute);

  double Sigmoid(double x);

  // Probability that "attacker" wins a duel against "defender" -- used for
  // zone advancement, shot outcomes, and first-possession rolls alike.
  double
  DuelProbability(double effectiveAttacker, double effectiveDefender, double steepness = 0.06);

  // ---- Engine ---------------------------------------------------------------

  class MatchEngine
  {
  public:
    MatchEngine(TeamStats home,
                TeamStats away,
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

    TeamStats m_home;
    TeamStats m_away;
    MatchContext m_context;
    Rng m_rng;
  };

} // namespace footbsim
