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
  //
  // Before feeding into the sigmoid, the raw stat gap is first compressed
  // by DUEL_GAP_COMPRESSION_EXPONENT (gap -> sign(gap) * |gap|^exponent).
  // With an exponent < 1, small gaps lose relatively little magnitude but
  // large gaps get compressed hard -- e.g. at 0.7, a gap of 5 shrinks to
  // ~3.1 (62%) while a gap of 50 shrinks to ~15.5 (31%). This exists
  // because a single linear steepness can't tell "slightly ahead on every
  // stat" apart from "wildly ahead": both are just "some gap size", and
  // the engine's own per-minute duel repetition (~90 independent rolls a
  // match) turns even a linearly-modest gap into near-certainty by full
  // time. Compression lets genuinely small gaps (like home advantage)
  // keep mattering while genuinely large ones stop running away to 100%.
  constexpr double DUEL_GAP_COMPRESSION_EXPONENT = 0.7;

  constexpr double KICKOFF_DUEL_STEEPNESS = 0.09; // who starts with the ball
  constexpr double ZONE_DUEL_STEEPNESS =
    0.058; // midfield advance & attacking-zone pressure (DuelProbability's default)
  constexpr double GOAL_DUEL_STEEPNESS = 0.058; // shot -> goal conversion

  // Baseline bias (added inside the sigmoid, before the steepness/gap term)
  // toward the possessing side for the two zone-flow duels only -- real
  // passing success is well above a coin flip, so two evenly-matched sides
  // shouldn't turn the ball over ~50% of the time by default. Not applied
  // to the kickoff or goal-conversion duels, which should stay a true
  // coin flip / pure stat contest between equal sides.
  constexpr double POSSESSION_RETENTION_BIAS = 0.6; // ~65% retention baseline for equal sides

  // ---- Possession flow ---------------------------------------------------
  // How readily a team in a phase of play pushes forward rather than just
  // recycling possession.
  constexpr double ZONE_ADVANCE_FRACTION =
    0.98; // fraction of a won midfield duel that pushes into the attacking third
  constexpr double SHOT_EAGERNESS =
    0.99; // fraction of a won attacking duel that becomes a shot attempt

  // ---- Shot outcomes ------------------------------------------------------
  // A shot resolves into goal / save / block / miss. SAVE_BAND + BLOCK_BAND
  // caps how many shots can ever count as "on target" or "blocked"; whatever
  // probability mass is left over is a miss. Saved/blocked shots each have
  // their own independent chance of producing a corner instead.
  constexpr double GOAL_CONVERSION_SCALE =
    0.21;                             // scales duel probability down to a goals/shots rate
  constexpr double SAVE_BAND = 0.25;  // shot-outcome mass: saved (on target, not a goal)
  constexpr double BLOCK_BAND = 0.15; // shot-outcome mass: blocked (off target)
  constexpr double SAVE_CORNER_CHANCE =
    0.9; // of saved shots, fraction that become a corner instead
  constexpr double BLOCK_CORNER_CHANCE =
    0.95; // of blocked shots, fraction that become a corner instead
  constexpr double PROBE_CORNER_CHANCE = 0.85; // a non-shot attacking probe can still win a corner

  // ---- Fouls & cards ------------------------------------------------------
  // A foul check runs every minute for whichever team doesn't have the
  // ball; a foul can then escalate into a card, and a repeat card can
  // escalate into a red.
  constexpr double FOUL_BASE_RATE = 0.29; // baseline per-minute foul chance for the defending team
  constexpr double FOUL_DISCIPLINE_BASE =
    0.3; // foul-rate floor even for a perfectly disciplined team
  constexpr double FOUL_DISCIPLINE_SCALE =
    1.4;                                  // how much poor discipline (0..1) amplifies the foul rate
  constexpr double CARD_BASE_RATE = 0.22; // baseline chance a foul is carded
  constexpr double CARD_DISCIPLINE_BASE =
    0.5; // card-rate floor even for a perfectly disciplined team
  constexpr double SECOND_YELLOW_CHANCE =
    0.035; // chance a repeat card is a second yellow rather than a fresh yellow
  constexpr double STRAIGHT_RED_CHANCE =
    0.006; // chance a first card is a straight red rather than a yellow
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

  // ---- Home advantage levels -----------------------------------------
  // Discrete crowd/venue advantage for the home side, from a neutral venue
  // (NONE) to a true fortress (MAXIMUM). MODERATE is the default and is
  // what the realism tuning pass above was calibrated against.
  constexpr double HOME_ADVANTAGE_NONE = 1.00; // neutral venue, e.g. a cup final on neutral ground
  constexpr double HOME_ADVANTAGE_SLIGHT = 1.22;   // mild edge
  constexpr double HOME_ADVANTAGE_MODERATE = 1.26; // typical top-flight home crowd (default)
  constexpr double HOME_ADVANTAGE_STRONG = 1.30;   // loud, hostile away experience
  constexpr double HOME_ADVANTAGE_MAXIMUM = 1.34;  // true fortress venue

  // ---- Referee style ---------------------------------------------------
  // Discrete card-issuing characteristic for the match official. BALANCED
  // is the default and matches the referee_strictness == 1.0 multiplier
  // the realism tuning pass above was calibrated against.
  constexpr double REFEREE_STYLE_LENIENT = 0.70;  // plays advantage, reluctant to book
  constexpr double REFEREE_STYLE_BALANCED = 1.00; // standard, by-the-book (default)
  constexpr double REFEREE_STYLE_STRICT = 1.35;   // low tolerance, quick whistle
  constexpr double REFEREE_STYLE_VERY_STRICT =
    1.70; // card-happy, stops play for the smallest contact

  // ---- Travel fatigue levels --------------------------------------------
  // Stamina penalty (subtracted from base stamina before the decay curve
  // above) for the away side, keyed to how far they traveled to play.
  // Escalates same city < different state < different region < different
  // country -- a region spans multiple states, so crossing into a
  // different region is a bigger trip than a neighboring state in the
  // same region. Not yet wired to real team geography (see
  // MatchContext::travel_fatigue); callers pick a level directly for now.
  // Deliberately small across the board: this is meant as a light nudge
  // on top of the match, not a dominant factor, so even DIFFERENT_COUNTRY
  // stays well under half of STAMINA_DECAY's own effect range.
  constexpr double TRAVEL_FATIGUE_SAME_CITY = 0.0;         // no penalty (default)
  constexpr double TRAVEL_FATIGUE_DIFFERENT_STATE = 2.0;   // short domestic trip
  constexpr double TRAVEL_FATIGUE_DIFFERENT_REGION = 5.0;  // longer domestic trip
  constexpr double TRAVEL_FATIGUE_DIFFERENT_COUNTRY = 8.0; // international travel

  // ---- External influences (MatchContext defaults) -----------------------
  // Neutral/realistic defaults for match-level context; callers override
  // per match as needed.
  constexpr double RAIN_ACCURACY_PENALTY =
    0.15; // max attack-accuracy penalty at full rain severity
  constexpr double WIND_ACCURACY_PENALTY =
    0.10; // max attack-accuracy penalty at full wind severity
  constexpr double SNOW_ACCURACY_PENALTY =
    0.20;                                  // max attack-accuracy penalty at full snow severity
  constexpr double RIVALRY_VARIANCE = 1.3; // morale-noise multiplier for a rivalry match
  constexpr double FINAL_VARIANCE = 1.6;   // morale-noise multiplier for a final

} // namespace footbsim::tuning
