#include "footbsim/match_engine.hpp"

#include <algorithm>
#include <cmath>

namespace footbsim
{

  // ---- Pure statistical primitives -----------------------------------------

  double EffectiveAttack(const TeamStats& stats,
                         double staminaFactor,
                         double homeFactor,
                         double weatherFactor,
                         double moraleNoise)
  {
    const double form_factor = 1.0 + stats.form / 100.0;
    const double base = stats.attack * form_factor * staminaFactor * homeFactor * weatherFactor;
    return std::max(1.0, base + moraleNoise);
  }

  double EffectiveDefense(const TeamStats& stats,
                          double staminaFactor,
                          double homeFactor,
                          double moraleNoise)
  {
    const double form_factor = 1.0 + stats.form / 200.0;
    const double base = stats.defense * form_factor * staminaFactor * homeFactor;
    return std::max(1.0, base + moraleNoise);
  }

  double StaminaDecayFactor(double baseStamina, int minute)
  {
    const double minute_frac = std::clamp(static_cast<double>(minute) / 90.0, 0.0, 1.0);
    const double fatigue_susceptibility = std::clamp((100.0 - baseStamina) / 100.0, 0.0, 1.0);
    const double decay = minute_frac * minute_frac * (0.05 + 0.20 * fatigue_susceptibility);
    return std::clamp(1.0 - decay, 0.5, 1.0);
  }

  double Sigmoid(double x)
  {
    return 1.0 / (1.0 + std::exp(-x));
  }

  double DuelProbability(double effectiveAttacker, double effectiveDefender, double steepness)
  {
    return Sigmoid(steepness * (effectiveAttacker - effectiveDefender));
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
    const double home_mid = m_home.midfield * m_context.home_advantage;
    const double away_mid = m_away.midfield;
    return m_rng.Bernoulli(DuelProbability(home_mid, away_mid, 0.08));
  }

  MatchEngine::MinuteEffectiveStats MatchEngine::ComputeMinuteStats(bool homeHasBall, int minute)
  {
    const TeamStats& possessing = homeHasBall ? m_home : m_away;
    const TeamStats& defending = homeHasBall ? m_away : m_home;

    const double travel_penalty_possessing = homeHasBall ? 0.0 : m_context.travel_fatigue;
    const double travel_penalty_defending = homeHasBall ? m_context.travel_fatigue : 0.0;

    const double stam_poss =
      StaminaDecayFactor(possessing.stamina - travel_penalty_possessing, minute);
    const double stam_def =
      StaminaDecayFactor(defending.stamina - travel_penalty_defending, minute);

    const double home_boost = homeHasBall ? m_context.home_advantage : 1.0;
    const double weather_factor =
      WeatherAccuracyFactor(m_context.weather, m_context.weather_severity);
    const double variance = StakesVarianceFactor(m_context.stakes);

    const double poss_noise = m_rng.Normal(0.0, variance * (100.0 - possessing.morale) / 20.0);
    const double def_noise = m_rng.Normal(0.0, variance * (100.0 - defending.morale) / 20.0);

    MinuteEffectiveStats eff;
    eff.possessing_attack =
      EffectiveAttack(possessing, stam_poss, home_boost, weather_factor, poss_noise);
    eff.possessing_midfield = possessing.midfield * stam_poss * home_boost;
    eff.defending_defense = EffectiveDefense(defending, stam_def, 1.0, def_noise);
    eff.defending_midfield = defending.midfield * stam_def;
    return eff;
  }

  void MatchEngine::MaybeGenerateFoul(bool homeIsDefending, int minute, MatchResult& result)
  {
    const TeamStats& defending = homeIsDefending ? m_home : m_away;
    TeamMatchState& def_state = homeIsDefending ? result.home_state : result.away_state;

    const double discipline_factor = std::clamp((100.0 - defending.discipline) / 100.0, 0.0, 1.0);
    const double p_foul = 0.02 * (0.3 + 1.4 * discipline_factor);

    if (!m_rng.Bernoulli(p_foul))
    {
      return;
    }

    def_state.fouls++;
    result.log.push_back({ minute,
                           EventType::FOUL,
                           defending.name,
                           PitchZone::MIDFIELD,
                           defending.name + " concede a foul" });

    const double p_card =
      std::clamp(0.15 * m_context.referee_strictness * (0.5 + discipline_factor), 0.0, 1.0);
    if (!m_rng.Bernoulli(p_card))
    {
      return;
    }

    const bool second_yellow = def_state.yellow_cards >= 1 && m_rng.Bernoulli(0.5);
    const bool straight_red = !second_yellow && m_rng.Bernoulli(0.08);

    if (second_yellow || straight_red)
    {
      def_state.red_cards++;
      def_state.has_red_card = true;
      const char* reason = second_yellow ? "second yellow" : "straight red";
      result.log.push_back({ minute,
                             EventType::RED_CARD,
                             defending.name,
                             PitchZone::MIDFIELD,
                             defending.name + " sent off (" + reason + ")" });
    }
    else
    {
      def_state.yellow_cards++;
      result.log.push_back({ minute,
                             EventType::YELLOW_CARD,
                             defending.name,
                             PitchZone::MIDFIELD,
                             defending.name + " booked" });
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

    if (roll < p_advance * 0.45)
    {
      zone = PitchZone::ATTACKING;
      result.log.push_back({ minute,
                             EventType::PASS,
                             possessing.name,
                             zone,
                             possessing.name + " advance into the attacking third" });
    }
    else if (roll < p_advance)
    {
      result.log.push_back({ minute,
                             EventType::PASS,
                             possessing.name,
                             zone,
                             possessing.name + " keep possession in midfield" });
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
                             new_possessor.name + " win the ball back in midfield" });
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
                             defending.name + " clear their lines" });
      return;
    }

    if (roll >= p_pressure * 0.35)
    {
      result.log.push_back({ minute,
                             EventType::PASS,
                             possessing.name,
                             zone,
                             possessing.name + " probe for an opening" });
      return;
    }

    // A shot happens.
    poss_state.shots++;
    result.log.push_back(
      { minute, EventType::SHOT, possessing.name, zone, possessing.name + " take a shot" });

    const double p_goal =
      DuelProbability(eff.possessing_attack, eff.defending_defense, 0.09) * 0.32;
    const double shot_roll = m_rng.Uniform01();

    if (shot_roll < p_goal)
    {
      poss_state.shots_on_target++;
      poss_state.goals++;
      result.log.push_back(
        { minute, EventType::GOAL, possessing.name, zone, "GOAL for " + possessing.name + "!" });
      homeHasBall = !homeHasBall;
      zone = PitchZone::MIDFIELD;
      return;
    }

    if (shot_roll < p_goal + 0.25)
    {
      poss_state.shots_on_target++;
      if (m_rng.Bernoulli(0.3))
      {
        poss_state.corners++;
        result.log.push_back(
          { minute, EventType::CORNER, possessing.name, zone, possessing.name + " win a corner" });
        // Stay in the attacking zone; corner delivery resolves next tick.
        return;
      }
      result.log.push_back(
        { minute, EventType::SAVE, defending.name, zone, defending.name + " keeper makes a save" });
      homeHasBall = !homeHasBall;
      zone = PitchZone::MIDFIELD;
      return;
    }

    if (shot_roll < p_goal + 0.25 + 0.15)
    {
      result.log.push_back(
        { minute, EventType::BLOCK, defending.name, zone, defending.name + " block the shot" });
      if (m_rng.Bernoulli(0.4))
      {
        poss_state.corners++;
        result.log.push_back(
          { minute, EventType::CORNER, possessing.name, zone, possessing.name + " win a corner" });
        return;
      }
      homeHasBall = !homeHasBall;
      zone = PitchZone::MIDFIELD;
      return;
    }

    result.log.push_back(
      { minute, EventType::MISS, possessing.name, zone, possessing.name + " shot goes wide" });
    homeHasBall = !homeHasBall;
    zone = PitchZone::MIDFIELD;
  }

  MatchResult MatchEngine::Simulate()
  {
    MatchResult result;
    result.log.push_back({ 0, EventType::KICKOFF, "", PitchZone::MIDFIELD, "Kickoff" });

    bool home_has_ball = DecideFirstPossession();
    PitchZone zone = PitchZone::MIDFIELD;

    for (int minute = 1; minute <= 90; ++minute)
    {
      if (minute == 45)
      {
        result.log.push_back({ 45, EventType::HALF_TIME, "", zone, "Half-time" });
      }

      TeamMatchState& poss_state = home_has_ball ? result.home_state : result.away_state;
      poss_state.possession_ticks++;

      MaybeGenerateFoul(home_has_ball, minute, result);

      MinuteEffectiveStats eff = ComputeMinuteStats(home_has_ball, minute);

      // Apply a lasting penalty for a team playing with a man down.
      const bool poss_has_red =
        (home_has_ball ? result.home_state : result.away_state).has_red_card;
      const bool def_has_red = (home_has_ball ? result.away_state : result.home_state).has_red_card;
      if (poss_has_red)
      {
        eff.possessing_attack *= 0.85;
        eff.possessing_midfield *= 0.85;
      }
      if (def_has_red)
      {
        eff.defending_defense *= 0.85;
        eff.defending_midfield *= 0.85;
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

    result.log.push_back({ 90, EventType::FULL_TIME, "", zone, "Full-time" });
    result.home_goals = result.home_state.goals;
    result.away_goals = result.away_state.goals;
    return result;
  }

} // namespace footbsim
