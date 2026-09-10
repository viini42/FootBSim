#include "footbsim/events.hpp"

namespace footbsim
{

  std::string ToString(EventType type)
  {
    switch (type)
    {
    case EventType::KICKOFF:
      return "Kickoff";
    case EventType::PASS:
      return "Pass";
    case EventType::TURNOVER:
      return "Turnover";
    case EventType::SHOT:
      return "Shot";
    case EventType::GOAL:
      return "GOAL";
    case EventType::SAVE:
      return "Save";
    case EventType::BLOCK:
      return "Block";
    case EventType::MISS:
      return "Miss";
    case EventType::CORNER:
      return "Corner";
    case EventType::FOUL:
      return "Foul";
    case EventType::YELLOW_CARD:
      return "Yellow Card";
    case EventType::RED_CARD:
      return "RED CARD";
    case EventType::HALF_TIME:
      return "Half-time";
    case EventType::FULL_TIME:
      return "Full-time";
    }
    return "Unknown";
  }

} // namespace footbsim
