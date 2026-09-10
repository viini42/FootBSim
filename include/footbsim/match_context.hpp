#pragma once

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

  // External influences on a match -- deliberately kept separate from
  // TeamStats since these describe the match, not the team.
  struct MatchContext
  {
    HomeAdvantageLevel home_advantage = HomeAdvantageLevel::MODERATE;
    Weather weather = Weather::CLEAR;
    double weather_severity = 0.0;   // 0..1, scales weather's accuracy penalty
    double referee_strictness = 1.0; // multiplier on foul -> card probability
    Stakes stakes = Stakes::NORMAL;
    double travel_fatigue = 0.0; // extra stamina penalty (0..100) applied to the away team
  };

  // Multiplier on the home team's effective attack/midfield for the given level.
  double HomeAdvantageMultiplier(HomeAdvantageLevel level);

  // Multiplier applied to attacking accuracy; 1.0 = no penalty.
  double WeatherAccuracyFactor(Weather weather, double severity);

  // Multiplier applied to morale-driven noise; >1.0 means more chaotic matches.
  double StakesVarianceFactor(Stakes stakes);

} // namespace footbsim
