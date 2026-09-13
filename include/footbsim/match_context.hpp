#pragma once

#include "footbsim/team_stats.hpp"
#include "footbsim/tuning.hpp"

namespace footbsim
{

  enum class Weather
  {
    CLEAR,
    RAIN,
    WIND,
    SNOW
  };
  enum class Stakes
  {
    NORMAL,
    RIVALRY,
    FINAL
  };

  // Discrete crowd/venue advantage for the home side, from a neutral venue
  // to a true fortress. Callers pick a level rather than a raw multiplier;
  // HomeAdvantageMultiplier() does the level -> number conversion.
  enum class HomeAdvantageLevel
  {
    NONE,
    SLIGHT,
    MODERATE,
    STRONG,
    MAXIMUM
  };

  // Card-issuing characteristic of the match official. Callers pick a
  // characteristic rather than a raw multiplier; RefereeStrictnessMultiplier()
  // does the characteristic -> number conversion.
  enum class RefereeStyle
  {
    LENIENT,
    BALANCED,
    STRICT,
    VERY_STRICT
  };

  // How far the away side traveled to play, from a same-city derby to a
  // different country entirely. TravelFatiguePenalty() does the level ->
  // number conversion. Kept deliberately subtle: see tuning.hpp.
  enum class TravelFatigueLevel
  {
    SAME_CITY,
    DIFFERENT_CITY,
    DIFFERENT_STATE,
    DIFFERENT_REGION,
    DIFFERENT_COUNTRY
  };

  // External influences on a match -- deliberately kept separate from
  // TeamStats since these describe the match, not the team.
  struct MatchContext
  {
    // Derives travel_fatigue from home/away's city/state/region (see
    // DetermineTravelFatigue() in match_context.cpp); every other field
    // keeps its neutral default below. Overwrite fields afterward for a
    // non-default match (e.g. context.stakes = Stakes::FINAL).
    MatchContext(const TeamStats& home, const TeamStats& away);

    HomeAdvantageLevel home_advantage = HomeAdvantageLevel::MODERATE;
    Weather weather = Weather::CLEAR;
    double weather_severity = 0.0; // 0..1, scales weather's accuracy penalty
    RefereeStyle referee_strictness = RefereeStyle::BALANCED;
    Stakes stakes = Stakes::NORMAL;
    TravelFatigueLevel travel_fatigue;
  };

  // Multiplier on the home team's effective attack/midfield for the given level.
  double HomeAdvantageMultiplier(HomeAdvantageLevel level);

  // Multiplier on foul -> card probability for the given referee style.
  double RefereeStrictnessMultiplier(RefereeStyle style);

  // Stamina penalty (0..100 scale) applied to the away team for the given travel level.
  double TravelFatiguePenalty(TravelFatigueLevel level);

  // Multiplier applied to attacking accuracy; 1.0 = no penalty.
  double WeatherAccuracyFactor(Weather weather, double severity);

  // Multiplier applied to morale-driven noise; >1.0 means more chaotic matches.
  double StakesVarianceFactor(Stakes stakes);

} // namespace footbsim
