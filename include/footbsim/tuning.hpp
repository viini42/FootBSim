#pragma once

namespace footbsim::tuning
{

  // All match-simulation "magic numbers" live here, grouped by what they
  // control together, so the whole probability model can be read and
  // recalibrated in one place instead of hunting across the engine.
  //
  // Values were calibrated by running thousands of seeded matches per
  // team-strength archetype and comparing aggregate stats against
  // real-world per-team-per-match averages (~11 shots, ~35% on target,
  // ~10-12% goals/shot, ~4 corners, ~11 fouls, ~1.7 yellow cards, ~2.5-2.7
  // combined goals, ~45/27/28 home/draw/away split for evenly matched
  // sides). See tests/test_match_engine_smoke.cpp for the aggregate
  // sanity checks that guard these.

  // ---- Duel steepness --------------------------------------------------
  // How sharply a stat gap between two teams turns into a probability
  // edge inside DuelProbability's sigmoid. Higher = a given gap decides
  // the outcome more often. Kept separate per contest because the same
  // gap size shouldn't necessarily matter equally at kickoff, in open
  // play, and in front of goal.
  constexpr double KICKOFF_DUEL_STEEPNESS = 0.055; // who starts with the ball
  constexpr double ZONE_DUEL_STEEPNESS =
    0.045; // midfield advance & attacking-zone pressure (DuelProbability's default)
  constexpr double GOAL_DUEL_STEEPNESS = 0.045; // shot -> goal conversion

  // ---- Possession flow ---------------------------------------------------
  // How readily a team in a phase of play pushes forward rather than just
  // recycling possession.
  constexpr double ZONE_ADVANCE_FRACTION =
    0.95; // fraction of a won midfield duel that pushes into the attacking third
  constexpr double SHOT_EAGERNESS =
    0.97; // fraction of a won attacking duel that becomes a shot attempt

  // ---- Shot outcomes ------------------------------------------------------
  // A shot resolves into goal / save / block / miss. SAVE_BAND + BLOCK_BAND
  // caps how many shots can ever count as "on target" or "blocked"; whatever
  // probability mass is left over is a miss. Saved/blocked shots each have
  // their own independent chance of producing a corner instead.
  constexpr double GOAL_CONVERSION_SCALE =
    0.26;                             // scales duel probability down to a goals/shots rate
  constexpr double SAVE_BAND = 0.25;  // shot-outcome mass: saved (on target, not a goal)
  constexpr double BLOCK_BAND = 0.15; // shot-outcome mass: blocked (off target)
  constexpr double SAVE_CORNER_CHANCE =
    0.9; // of saved shots, fraction that become a corner instead
  constexpr double BLOCK_CORNER_CHANCE =
    0.95; // of blocked shots, fraction that become a corner instead
  constexpr double PROBE_CORNER_CHANCE = 0.7; // a non-shot attacking probe can still win a corner

  // ---- Fouls & cards ------------------------------------------------------
  // A foul check runs every minute for whichever team doesn't have the
  // ball; a foul can then escalate into a card, and a repeat card can
  // escalate into a red.
  constexpr double FOUL_BASE_RATE = 0.26; // baseline per-minute foul chance for the defending team
  constexpr double FOUL_DISCIPLINE_BASE =
    0.3; // foul-rate floor even for a perfectly disciplined team
  constexpr double FOUL_DISCIPLINE_SCALE =
    1.4;                                  // how much poor discipline (0..1) amplifies the foul rate
  constexpr double CARD_BASE_RATE = 0.20; // baseline chance a foul is carded
  constexpr double CARD_DISCIPLINE_BASE =
    0.5; // card-rate floor even for a perfectly disciplined team
  constexpr double SECOND_YELLOW_CHANCE =
    0.08; // chance a repeat card is a second yellow rather than a fresh yellow
  constexpr double STRAIGHT_RED_CHANCE =
    0.015; // chance a first card is a straight red rather than a yellow
  constexpr double RED_CARD_PENALTY =
    0.85; // effective-stat multiplier for a team playing a man down

  // ---- Stamina decay ------------------------------------------------------
  // Effective stats fade as the match wears on; teams with worse base
  // stamina fade faster and further.
  constexpr double STAMINA_DECAY_BASE =
    0.05; // decay applied even to a maximum-stamina team by minute 90
  constexpr double STAMINA_DECAY_FATIGUE_SCALE =
    0.20; // extra decay per point of fatigue susceptibility (0..1)
  constexpr double STAMINA_DECAY_FLOOR =
    0.5; // effective stats never drop below this fraction of base

  // ---- Form -----------------------------------------------------------
  // How much recent-result momentum (-10..+10) moves each effective stat;
  // attack is more sensitive to form than defense.
  constexpr double ATTACK_FORM_SCALE = 100.0;
  constexpr double DEFENSE_FORM_SCALE = 200.0;

  // ---- Tactical aggression ------------------------------------------------
  // Trade-off curves for TeamStats::aggression (0..100, neutral at 50):
  // attacking tactics boost attack and weaken defense; pressing raises both
  // shot eagerness and foul rate. See AggressionAttackFactor /
  // AggressionDefenseFactor / AggressionPressureFactor.
  constexpr double AGGRESSION_ATTACK_SCALE =
    200.0; // 0.75 (fully defensive) .. 1.25 (all-out attack)
  constexpr double AGGRESSION_DEFENSE_SCALE =
    250.0; // 1.2 (fully defensive) .. 0.8 (all-out attack)
  constexpr double AGGRESSION_PRESSURE_SCALE =
    250.0; // 0.8 (fully defensive) .. 1.2 (all-out attack)

  // ---- External influences (MatchContext defaults) -----------------------
  // Neutral/realistic defaults for match-level context; callers override
  // per match as needed.
  constexpr double DEFAULT_HOME_ADVANTAGE =
    1.07; // multiplier on home team's effective attack/midfield
  constexpr double RAIN_ACCURACY_PENALTY =
    0.15; // max attack-accuracy penalty at full rain severity
  constexpr double WIND_ACCURACY_PENALTY =
    0.10; // max attack-accuracy penalty at full wind severity
  constexpr double SNOW_ACCURACY_PENALTY =
    0.20;                                  // max attack-accuracy penalty at full snow severity
  constexpr double RIVALRY_VARIANCE = 1.3; // morale-noise multiplier for a rivalry match
  constexpr double FINAL_VARIANCE = 1.6;   // morale-noise multiplier for a final

} // namespace footbsim::tuning
