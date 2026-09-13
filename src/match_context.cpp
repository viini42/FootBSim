#include "footbsim/match_context.hpp"

#include "footbsim/tuning.hpp"

#include <algorithm>

namespace footbsim
{

  namespace
  {

    // Buckets a home/away pair into a TravelFatigueLevel from their
    // city/state/region: same city implies same state implies same
    // region, so checking city, then state, then region in order covers
    // every domestically reachable level. DIFFERENT_COUNTRY needs a
    // country field this data doesn't have, so it stays available only
    // for a caller to set by hand.
    TravelFatigueLevel DetermineTravelFatigue(const TeamStats& home, const TeamStats& away)
    {
      if (home.city == away.city)
      {
        return TravelFatigueLevel::SAME_CITY;
      }
      if (home.state == away.state)
      {
        return TravelFatigueLevel::DIFFERENT_CITY;
      }
      if (home.region == away.region)
      {
        return TravelFatigueLevel::DIFFERENT_STATE;
      }
      return TravelFatigueLevel::DIFFERENT_REGION;
    }

  } // namespace

  MatchContext::MatchContext(const TeamStats& home, const TeamStats& away)
    : travel_fatigue(DetermineTravelFatigue(home, away))
  {
  }

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

  double RefereeStrictnessMultiplier(RefereeStyle style)
  {
    switch (style)
    {
    case RefereeStyle::LENIENT:
      return tuning::REFEREE_STYLE_LENIENT;
    case RefereeStyle::BALANCED:
      return tuning::REFEREE_STYLE_BALANCED;
    case RefereeStyle::STRICT:
      return tuning::REFEREE_STYLE_STRICT;
    case RefereeStyle::VERY_STRICT:
      return tuning::REFEREE_STYLE_VERY_STRICT;
    }
    return tuning::REFEREE_STYLE_BALANCED;
  }

  double TravelFatiguePenalty(TravelFatigueLevel level)
  {
    switch (level)
    {
    case TravelFatigueLevel::SAME_CITY:
      return tuning::TRAVEL_FATIGUE_SAME_CITY;
    case TravelFatigueLevel::DIFFERENT_CITY:
      return tuning::TRAVEL_FATIGUE_DIFFERENT_CITY;
    case TravelFatigueLevel::DIFFERENT_REGION:
      return tuning::TRAVEL_FATIGUE_DIFFERENT_REGION;
    case TravelFatigueLevel::DIFFERENT_STATE:
      return tuning::TRAVEL_FATIGUE_DIFFERENT_STATE;
    case TravelFatigueLevel::DIFFERENT_COUNTRY:
      return tuning::TRAVEL_FATIGUE_DIFFERENT_COUNTRY;
    }
    return tuning::TRAVEL_FATIGUE_SAME_CITY;
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
