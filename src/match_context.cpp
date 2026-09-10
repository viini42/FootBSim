#include "footbsim/match_context.hpp"

#include "footbsim/tuning.hpp"

#include <algorithm>

namespace footbsim
{

  double HomeAdvantageMultiplier(HomeAdvantageLevel level)
  {
    switch (level)
    {
    case HomeAdvantageLevel::NONE:
      return tuning::HOME_ADVANTAGE_NONE;
    case HomeAdvantageLevel::SLIGHT:
      return tuning::HOME_ADVANTAGE_SLIGHT;
    case HomeAdvantageLevel::MODERATE:
      return tuning::HOME_ADVANTAGE_MODERATE;
    case HomeAdvantageLevel::STRONG:
      return tuning::HOME_ADVANTAGE_STRONG;
    case HomeAdvantageLevel::MAXIMUM:
      return tuning::HOME_ADVANTAGE_MAXIMUM;
    }
    return tuning::HOME_ADVANTAGE_MODERATE;
  }

  double WeatherAccuracyFactor(Weather weather, double severity)
  {
    const double s = std::clamp(severity, 0.0, 1.0);
    switch (weather)
    {
    case Weather::CLEAR:
      return 1.0;
    case Weather::RAIN:
      return 1.0 - tuning::RAIN_ACCURACY_PENALTY * s;
    case Weather::WIND:
      return 1.0 - tuning::WIND_ACCURACY_PENALTY * s;
    case Weather::SNOW:
      return 1.0 - tuning::SNOW_ACCURACY_PENALTY * s;
    }
    return 1.0;
  }

  double StakesVarianceFactor(Stakes stakes)
  {
    switch (stakes)
    {
    case Stakes::NORMAL:
      return 1.0;
    case Stakes::RIVALRY:
      return tuning::RIVALRY_VARIANCE;
    case Stakes::FINAL:
      return tuning::FINAL_VARIANCE;
    }
    return 1.0;
  }

} // namespace footbsim
