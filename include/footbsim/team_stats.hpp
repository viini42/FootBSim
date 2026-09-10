#pragma once

#include <string>

namespace footbsim
{

  // Team-level stats only -- no individual players. Ranges are documented per
  // field; the engine does not clamp these on construction, callers should
  // keep them sane.
  struct TeamStats
  {
    std::string name;

    double attack = 50.0;     // 0-100: chance creation & finishing quality
    double defense = 50.0;    // 0-100: ability to resist opponent attacks
    double midfield = 50.0;   // 0-100: possession/tempo control
    double form = 0.0;        // -10..+10: recent-result momentum
    double stamina = 100.0;   // 0-100: squad condition at kickoff
    double discipline = 50.0; // 0-100 (higher = cleaner): foul/card tendency
    double morale = 50.0;     // 0-100: higher = more consistent performance
    double aggression = 50.0; // 0-100: tactical intent, 100 = all-out attack, 0 = fully defensive
  };

} // namespace footbsim
