#pragma once

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

  // External influences on a match -- deliberately kept separate from
  // TeamStats since these describe the match, not the team.
  struct MatchContext
  {
    double home_advantage = 1.12; // multiplier on home team's effective attack/midfield
    Weather weather = Weather::CLEAR;
    double weather_severity = 0.0;   // 0..1, scales weather's accuracy penalty
    double referee_strictness = 1.0; // multiplier on foul -> card probability
    Stakes stakes = Stakes::NORMAL;
    double travel_fatigue = 0.0; // extra stamina penalty (0..100) applied to the away team
  };

  // Multiplier applied to attacking accuracy; 1.0 = no penalty.
  double WeatherAccuracyFactor(Weather weather, double severity);

  // Multiplier applied to morale-driven noise; >1.0 means more chaotic matches.
  double StakesVarianceFactor(Stakes stakes);

} // namespace footbsim
