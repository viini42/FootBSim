#include "footbsim/team_config.hpp"

#include "footbsim/tuning.hpp"

namespace footbsim
{

  double FormationAggressionOffset(Formation formation)
  {
    switch (formation)
    {
    case Formation::FIVE_THREE_TWO:
      return tuning::FORMATION_FIVE_THREE_TWO_OFFSET;
    case Formation::FOUR_FOUR_TWO:
      return tuning::FORMATION_FOUR_FOUR_TWO_OFFSET;
    case Formation::THREE_FIVE_TWO:
      return tuning::FORMATION_THREE_FIVE_TWO_OFFSET;
    case Formation::FOUR_THREE_THREE:
      return tuning::FORMATION_FOUR_THREE_THREE_OFFSET;
    }
    return tuning::FORMATION_FOUR_FOUR_TWO_OFFSET;
  }

  double PressIntensityStaminaCost(PressIntensity press_intensity)
  {
    switch (press_intensity)
    {
    case PressIntensity::LOW_BLOCK:
      return tuning::PRESS_INTENSITY_LOW_BLOCK_STAMINA_COST;
    case PressIntensity::MEDIUM:
      return tuning::PRESS_INTENSITY_MEDIUM_STAMINA_COST;
    case PressIntensity::HIGH_PRESS:
      return tuning::PRESS_INTENSITY_HIGH_PRESS_STAMINA_COST;
    }
    return tuning::PRESS_INTENSITY_MEDIUM_STAMINA_COST;
  }

  double PressIntensityFoulMultiplier(PressIntensity press_intensity)
  {
    switch (press_intensity)
    {
    case PressIntensity::LOW_BLOCK:
      return tuning::PRESS_INTENSITY_LOW_BLOCK_FOUL_MULTIPLIER;
    case PressIntensity::MEDIUM:
      return tuning::PRESS_INTENSITY_MEDIUM_FOUL_MULTIPLIER;
    case PressIntensity::HIGH_PRESS:
      return tuning::PRESS_INTENSITY_HIGH_PRESS_FOUL_MULTIPLIER;
    }
    return tuning::PRESS_INTENSITY_MEDIUM_FOUL_MULTIPLIER;
  }

  double TempoAdvanceMultiplier(Tempo tempo)
  {
    switch (tempo)
    {
    case Tempo::PATIENT:
      return tuning::TEMPO_PATIENT_ADVANCE_MULTIPLIER;
    case Tempo::BALANCED:
      return tuning::TEMPO_BALANCED_ADVANCE_MULTIPLIER;
    case Tempo::DIRECT:
      return tuning::TEMPO_DIRECT_ADVANCE_MULTIPLIER;
    }
    return tuning::TEMPO_BALANCED_ADVANCE_MULTIPLIER;
  }

} // namespace footbsim
