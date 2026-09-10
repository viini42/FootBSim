#include "footbsim/match_context.hpp"

#include <algorithm>

namespace footbsim
{

  double WeatherAccuracyFactor(Weather weather, double severity)
  {
    const double s = std::clamp(severity, 0.0, 1.0);
    switch (weather)
    {
    case Weather::CLEAR:
      return 1.0;
    case Weather::RAIN:
      return 1.0 - 0.15 * s;
    case Weather::WIND:
      return 1.0 - 0.10 * s;
    case Weather::SNOW:
      return 1.0 - 0.20 * s;
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
      return 1.3;
    case Stakes::FINAL:
      return 1.6;
    }
    return 1.0;
  }

} // namespace footbsim
