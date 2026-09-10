#include "footbsim/match_engine.hpp"

#include "footbsim/messages.hpp"
#include "footbsim/tuning.hpp"

#include <algorithm>
#include <cmath>

namespace footbsim
{

  // ---- Pure statistical primitives -----------------------------------------

  double EffectiveAttack(const TeamStats& stats,
                         double staminaFactor,
                         double homeFactor,
                         double weatherFactor,
                         double aggressionFactor,
                         double moraleNoise)
  {
    const double form_factor = 1.0 + stats.form / tuning::ATTACK_FORM_SCALE;
    const double base =
      stats.attack * form_factor * staminaFactor * homeFactor * weatherFactor * aggressionFactor;
    return std::max(1.0, base + moraleNoise);
  }

  double EffectiveDefense(const TeamStats& stats,
                          double staminaFactor,
                          double homeFactor,
                          double aggressionFactor,
                          double moraleNoise)
  {
    const double form_factor = 1.0 + stats.form / tuning::DEFENSE_FORM_SCALE;
    const double base = stats.defense * form_factor * staminaFactor * homeFactor * aggressionFactor;
    return std::max(1.0, base + moraleNoise);
  }

  double StaminaDecayFactor(double baseStamina, int minute)
  {
    const double minute_frac = std::clamp(static_cast<double>(minute) / 90.0, 0.0, 1.0);
    const double fatigue_susceptibility = std::clamp((100.0 - baseStamina) / 100.0, 0.0, 1.0);
    const double decay =
      minute_frac * minute_frac *
      (tuning::STAMINA_DECAY_BASE + tuning::STAMINA_DECAY_FATIGUE_SCALE * fatigue_susceptibility);
    return std::clamp(1.0 - decay, tuning::STAMINA_DECAY_FLOOR, 1.0);
  }

  double AggressionAttackFactor(double aggression)
  {
    const double a = std::clamp(aggression, 0.0, 100.0);
    return 1.0 +
           (a - 50.0) /
             tuning::AGGRESSION_ATTACK_SCALE; // 0.75 (fully defensive) .. 1.25 (all-out attack)
  }

  double AggressionDefenseFactor(double aggression)
  {
    const double a = std::clamp(aggression, 0.0, 100.0);
    return 1.0 -
           (a - 50.0) /
             tuning::AGGRESSION_DEFENSE_SCALE; // 1.2 (fully defensive) .. 0.8 (all-out attack)
  }

  double AggressionPressureFactor(double aggression)
  {
    const double a = std::clamp(aggression, 0.0, 100.0);
    return 1.0 +
           (a - 50.0) /
             tuning::AGGRESSION_PRESSURE_SCALE; // 0.8 (fully defensive) .. 1.2 (all-out attack)
  }

  double Sigmoid(double x)
  {
    return 1.0 / (1.0 + std::exp(-x));
  }

  double DuelProbability(double effectiveAttacker, double effectiveDefender, double steepness)
  {
    const double gap = effectiveAttacker - effectiveDefender;
    const double compressed_gap =
      std::copysign(std::pow(std::abs(gap), tuning::DUEL_GAP_COMPRESSION_EXPONENT), gap);
    return Sigmoid(steepness * compressed_gap);
  }

  // ---- Engine ---------------------------------------------------------------

  MatchEngine::MatchEngine(TeamStats home,
                           TeamStats away,
                           MatchContext context,
                           std::optional<std::uint64_t> seed) :
      m_home(std::move(home)), m_away(std::move(away)), m_context(context), m_rng(seed)
  {
  }

  bool MatchEngine::DecideFirstPossession()
  {
    const double home_mid = m_home.midfield * HomeAdvantageMultiplier(m_context.home_advantage);
    const double away_mid = m_away.midfield;
    return m_rng.Bernoulli(DuelProbability(home_mid, away_mid, tuning::KICKOFF_DUEL_STEEPNESS));
  }

  MatchEngine::MinuteEffectiveStats MatchEngine::ComputeMinuteStats(bool homeHasBall, int minute)
  {
    const TeamStats& possessing = homeHasBall ? m_home : m_away;
    const TeamStats& defending = homeHasBall ? m_away : m_home;

    const double travel_penalty = TravelFatiguePenalty(m_context.travel_fatigue);
    const double travel_penalty_possessing = homeHasBall ? 0.0 : travel_penalty;
    const double travel_penalty_defending = homeHasBall ? travel_penalty : 0.0;

    const double stam_poss =
      StaminaDecayFactor(possessing.stamina - travel_penalty_possessing, minute);
    const double stam_def =
      StaminaDecayFactor(defending.stamina - travel_penalty_defending, minute);

    const double home_boost = homeHasBall ? HomeAdvantageMultiplier(m_context.home_advantage) : 1.0;
    const double weather_factor =
      WeatherAccuracyFactor(m_context.weather, m_context.weather_severity);
    const double variance = StakesVarianceFactor(m_context.stakes);

    const double poss_noise = m_rng.Normal(0.0, variance * (100.0 - possessing.morale) / 20.0);
    const double def_noise = m_rng.Normal(0.0, variance * (100.0 - defending.morale) / 20.0);

    const double poss_aggression_attack = AggressionAttackFactor(possessing.aggression);
    const double def_aggression_defense = AggressionDefenseFactor(defending.aggression);

    MinuteEffectiveStats eff;
    eff.possessing_attack = EffectiveAttack(possessing,
                                            stam_poss,
                                            home_boost,
                                            weather_factor,
                                            poss_aggression_attack,
                                            poss_noise);
    eff.possessing_midfield = possessing.midfield * stam_poss * home_boost;
    eff.defending_defense =
      EffectiveDefense(defending, stam_def, 1.0, def_aggression_defense, def_noise);
    eff.defending_midfield = defending.midfield * stam_def;
    return eff;
  }

  void MatchEngine::MaybeGenerateFoul(bool homeIsDefending, int minute, MatchResult& result)
  {
    const TeamStats& defending = homeIsDefending ? m_home : m_away;
    TeamMatchState& def_state = homeIsDefending ? result.home_state : result.away_state;

    const double discipline_factor = std::clamp((100.0 - defending.discipline) / 100.0, 0.0, 1.0);
    const double p_foul =
      tuning::FOUL_BASE_RATE *
      (tuning::FOUL_DISCIPLINE_BASE + tuning::FOUL_DISCIPLINE_SCALE * discipline_factor) *
      AggressionPressureFactor(defending.aggression);

    if (!m_rng.Bernoulli(p_foul))
    {
      return;
    }

    def_state.fouls++;
    result.log.push_back({ minute,
                           EventType::FOUL,
                           defending.name,
                           PitchZone::MIDFIELD,
                           messages::ConcedeFoul(defending.name) });

    const double p_card = std::clamp(tuning::CARD_BASE_RATE *
                                       RefereeStrictnessMultiplier(m_context.referee_strictness) *
                                       (tuning::CARD_DISCIPLINE_BASE + discipline_factor),
                                     0.0,
                                     1.0);
    if (!m_rng.Bernoulli(p_card))
    {
      return;
    }

    const bool second_yellow =
      def_state.yellow_cards >= 1 && m_rng.Bernoulli(tuning::SECOND_YELLOW_CHANCE);
    const bool straight_red = !second_yellow && m_rng.Bernoulli(tuning::STRAIGHT_RED_CHANCE);

    if (second_yellow || straight_red)
    {
      def_state.red_cards++;
      def_state.has_red_card = true;
      const std::string description = second_yellow ? messages::SentOffSecondYellow(defending.name)
                                                    : messages::SentOffStraightRed(defending.name);
      result.log.push_back(
        { minute, EventType::RED_CARD, defending.name, PitchZone::MIDFIELD, description });
    }
    else
    {
      def_state.yellow_cards++;
      result.log.push_back({ minute,
                             EventType::YELLOW_CARD,
                             defending.name,
                             PitchZone::MIDFIELD,
                             messages::Booked(defending.name) });
    }
  }

  void MatchEngine::ResolveMidfieldZone(bool& homeHasBall,
                                        PitchZone& zone,
                                        const MinuteEffectiveStats& eff,
                                        int minute,
                                        MatchResult& result)
  {
    const TeamStats& possessing = homeHasBall ? m_home : m_away;
    const double p_advance = DuelProbability(eff.possessing_midfield, eff.defending_midfield);
    const double roll = m_rng.Uniform01();

    if (roll < p_advance * tuning::ZONE_ADVANCE_FRACTION)
    {
      zone = PitchZone::ATTACKING;
      result.log.push_back({ minute,
                             EventType::PASS,
                             possessing.name,
                             zone,
                             messages::AdvanceToAttackingThird(possessing.name) });
    }
    else if (roll < p_advance)
    {
      result.log.push_back({ minute,
                             EventType::PASS,
                             possessing.name,
                             zone,
                             messages::KeepPossessionInMidfield(possessing.name) });
    }
    else
    {
      homeHasBall = !homeHasBall;
      zone = PitchZone::MIDFIELD;
      const TeamStats& new_possessor = homeHasBall ? m_home : m_away;
      result.log.push_back({ minute,
                             EventType::TURNOVER,
                             new_possessor.name,
                             zone,
                             messages::WinBallBackInMidfield(new_possessor.name) });
    }
  }

  void MatchEngine::ResolveAttackingZone(bool& homeHasBall,
                                         PitchZone& zone,
                                         const MinuteEffectiveStats& eff,
                                         int minute,
                                         MatchResult& result)
  {
    const TeamStats& possessing = homeHasBall ? m_home : m_away;
    const TeamStats& defending = homeHasBall ? m_away : m_home;
    TeamMatchState& poss_state = homeHasBall ? result.home_state : result.away_state;

    const double p_pressure = DuelProbability(eff.possessing_attack, eff.defending_defense);
    const double roll = m_rng.Uniform01();

    if (roll >= p_pressure)
    {
      homeHasBall = !homeHasBall;
      zone = PitchZone::MIDFIELD;
      result.log.push_back({ minute,
                             EventType::TURNOVER,
                             defending.name,
                             zone,
                             messages::ClearLines(defending.name) });
      return;
    }

    const double shot_threshold =
      p_pressure * tuning::SHOT_EAGERNESS * AggressionPressureFactor(possessing.aggression);
    if (roll >= shot_threshold)
    {
      if (m_rng.Bernoulli(tuning::PROBE_CORNER_CHANCE))
      {
        poss_state.corners++;
        result.log.push_back({ minute,
                               EventType::CORNER,
                               possessing.name,
                               zone,
                               messages::WinCorner(possessing.name) });
        return;
      }
      result.log.push_back({ minute,
                             EventType::PASS,
                             possessing.name,
                             zone,
                             messages::ProbeForOpening(possessing.name) });
      return;
    }

    // A shot happens.
    poss_state.shots++;
    result.log.push_back(
      { minute, EventType::SHOT, possessing.name, zone, messages::TakeShot(possessing.name) });

    const double p_goal =
      DuelProbability(eff.possessing_attack, eff.defending_defense, tuning::GOAL_DUEL_STEEPNESS) *
      tuning::GOAL_CONVERSION_SCALE;
    const double shot_roll = m_rng.Uniform01();

    if (shot_roll < p_goal)
    {
      poss_state.shots_on_target++;
      poss_state.goals++;
      result.log.push_back(
        { minute, EventType::GOAL, possessing.name, zone, messages::Goal(possessing.name) });
      homeHasBall = !homeHasBall;
      zone = PitchZone::MIDFIELD;
      return;
    }

    if (shot_roll < p_goal + tuning::SAVE_BAND)
    {
      poss_state.shots_on_target++;
      if (m_rng.Bernoulli(tuning::SAVE_CORNER_CHANCE))
      {
        poss_state.corners++;
        result.log.push_back({ minute,
                               EventType::CORNER,
                               possessing.name,
                               zone,
                               messages::WinCorner(possessing.name) });
        // Stay in the attacking zone; corner delivery resolves next tick.
        return;
      }
      result.log.push_back(
        { minute, EventType::SAVE, defending.name, zone, messages::KeeperSave(defending.name) });
      homeHasBall = !homeHasBall;
      zone = PitchZone::MIDFIELD;
      return;
    }

    if (shot_roll < p_goal + tuning::SAVE_BAND + tuning::BLOCK_BAND)
    {
      result.log.push_back(
        { minute, EventType::BLOCK, defending.name, zone, messages::BlockShot(defending.name) });
      if (m_rng.Bernoulli(tuning::BLOCK_CORNER_CHANCE))
      {
        poss_state.corners++;
        result.log.push_back({ minute,
                               EventType::CORNER,
                               possessing.name,
                               zone,
                               messages::WinCorner(possessing.name) });
        return;
      }
      homeHasBall = !homeHasBall;
      zone = PitchZone::MIDFIELD;
      return;
    }

    result.log.push_back(
      { minute, EventType::MISS, possessing.name, zone, messages::ShotWide(possessing.name) });
    homeHasBall = !homeHasBall;
    zone = PitchZone::MIDFIELD;
  }

  MatchResult MatchEngine::Simulate()
  {
    MatchResult result;
    result.log.push_back({ 0, EventType::KICKOFF, "", PitchZone::MIDFIELD, messages::Kickoff() });

    bool home_has_ball = DecideFirstPossession();
    PitchZone zone = PitchZone::MIDFIELD;

    for (int minute = 1; minute <= 90; ++minute)
    {
      if (minute == 45)
      {
        result.log.push_back({ 45, EventType::HALF_TIME, "", zone, messages::HalfTime() });
      }

      TeamMatchState& poss_state = home_has_ball ? result.home_state : result.away_state;
      poss_state.possession_ticks++;

      MaybeGenerateFoul(!home_has_ball, minute, result);

      MinuteEffectiveStats eff = ComputeMinuteStats(home_has_ball, minute);

      // Apply a lasting penalty for a team playing with a man down.
      const bool poss_has_red =
        (home_has_ball ? result.home_state : result.away_state).has_red_card;
      const bool def_has_red = (home_has_ball ? result.away_state : result.home_state).has_red_card;
      if (poss_has_red)
      {
        eff.possessing_attack *= tuning::RED_CARD_PENALTY;
        eff.possessing_midfield *= tuning::RED_CARD_PENALTY;
      }
      if (def_has_red)
      {
        eff.defending_defense *= tuning::RED_CARD_PENALTY;
        eff.defending_midfield *= tuning::RED_CARD_PENALTY;
      }

      if (zone == PitchZone::MIDFIELD)
      {
        ResolveMidfieldZone(home_has_ball, zone, eff, minute, result);
      }
      else
      {
        ResolveAttackingZone(home_has_ball, zone, eff, minute, result);
      }
    }

    result.log.push_back({ 90, EventType::FULL_TIME, "", zone, messages::FullTime() });
    result.home_goals = result.home_state.goals;
    result.away_goals = result.away_state.goals;
    return result;
  }

} // namespace footbsim
