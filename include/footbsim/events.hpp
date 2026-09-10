#pragma once

#include "footbsim/pitch_zone.hpp"

#include <string>

namespace footbsim
{

  enum class EventType
  {
    KICKOFF,
    PASS,
    TURNOVER,
    SHOT,
    GOAL,
    SAVE,
    BLOCK,
    MISS,
    CORNER,
    FOUL,
    YELLOW_CARD,
    RED_CARD,
    HALF_TIME,
    FULL_TIME,
  };

  struct MatchEvent
  {
    int minute = 0;
    EventType type{};
    std::string team; // team name this event is attributed to (empty for neutral events)
    PitchZone zone = PitchZone::MIDFIELD;
    std::string description;
  };

  std::string ToString(EventType type);

} // namespace footbsim
