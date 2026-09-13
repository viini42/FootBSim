#pragma once

#include "footbsim/team_stats.hpp"

namespace footbsim
{

  // Attacking/defensive mentality, expressed as a fixed set of named
  // formations a coach would recognize rather than an abstract slider.
  // Internally this masks a plain offset applied to TeamStats::aggression
  // for the match (see FormationAggressionOffset and EffectiveAggression
  // in match_engine.hpp) -- the same already-tuned AggressionAttackFactor /
  // AggressionDefenseFactor / AggressionPressureFactor curves still do all
  // the work. FOUR_FOUR_TWO is neutral and the default; the others lean
  // defensive or attacking. This pairing of formation names to intent is
  // illustrative, not a claim about how these formations actually play in
  // real football.
  enum class Formation
  {
    FIVE_THREE_TWO,
    FOUR_FOUR_TWO,
    THREE_FIVE_TWO,
    FOUR_THREE_THREE
  };

  // How hard a team presses when it doesn't have the ball. Costs the
  // pressing team stamina (same mechanism MatchContext::travel_fatigue
  // already uses to pre-adjust stamina before StaminaDecayFactor) and
  // raises their foul rate; MEDIUM is neutral and the default.
  enum class PressIntensity
  {
    LOW_BLOCK,
    MEDIUM,
    HIGH_PRESS
  };

  // How eagerly a team pushes forward once it wins a midfield duel, versus
  // recycling possession patiently. Scales ZONE_ADVANCE_FRACTION rather
  // than replacing it; BALANCED is neutral and the default.
  enum class Tempo
  {
    PATIENT,
    BALANCED,
    DIRECT
  };

  // One team's stats plus its coach-chosen tactics for one specific match.
  // TeamStats stays a pure "how good is this team" snapshot -- the same
  // one reused as-is across matches from a roster header or derived from a
  // season CSV. TeamConfig is what MatchEngine actually takes, pairing
  // that snapshot with the per-match choices a coach would make.
  struct TeamConfig
  {
    TeamStats stats;
    Formation formation = Formation::FOUR_FOUR_TWO;
    PressIntensity press_intensity = PressIntensity::MEDIUM;
    Tempo tempo = Tempo::BALANCED;
  };

  // Offset added to TeamStats::aggression for the duration of one match --
  // see Formation's own doc comment for the mechanism this feeds into.
  double FormationAggressionOffset(Formation formation);

  // Stamina points subtracted before StaminaDecayFactor for the team
  // currently pressing (i.e. without the ball).
  double PressIntensityStaminaCost(PressIntensity press_intensity);

  // Multiplier on the per-minute foul rate for the pressing team.
  double PressIntensityFoulMultiplier(PressIntensity press_intensity);

  // Multiplier on ZONE_ADVANCE_FRACTION: how much of a won midfield duel
  // pushes forward immediately versus recycling possession.
  double TempoAdvanceMultiplier(Tempo tempo);

} // namespace footbsim
