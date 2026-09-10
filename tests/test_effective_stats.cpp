#include "footbsim/match_engine.hpp"
#include "footbsim/team_stats.hpp"
#include "micro_test.hpp"

using footbsim::TeamStats;

FOOTBSIM_TEST(higher_attack_stat_yields_higher_effective_attack)
{
  TeamStats weak;
  weak.attack = 40;
  weak.form = 0;

  TeamStats strong = weak;
  strong.attack = 80;

  const double eff_weak = footbsim::EffectiveAttack(weak,
                                                    /*staminaFactor=*/1.0,
                                                    /*homeFactor=*/1.0,
                                                    /*weatherFactor=*/1.0,
                                                    /*moraleNoise=*/0.0);
  const double eff_strong = footbsim::EffectiveAttack(strong, 1.0, 1.0, 1.0, 0.0);

  FOOTBSIM_CHECK(eff_strong > eff_weak);
}

FOOTBSIM_TEST(stamina_decay_reduces_effective_stats_over_the_match)
{
  TeamStats stats;
  stats.attack = 70;
  stats.form = 0;

  const double early = footbsim::StaminaDecayFactor(stats.stamina, /*minute=*/5);
  const double late = footbsim::StaminaDecayFactor(stats.stamina, /*minute=*/90);

  FOOTBSIM_CHECK(late <= early);

  const double eff_early = footbsim::EffectiveAttack(stats, early, 1.0, 1.0, 0.0);
  const double eff_late = footbsim::EffectiveAttack(stats, late, 1.0, 1.0, 0.0);
  FOOTBSIM_CHECK(eff_late <= eff_early);
}

FOOTBSIM_TEST(poor_stamina_teams_decay_faster_than_fit_teams)
{
  const double fit_decay = footbsim::StaminaDecayFactor(/*baseStamina=*/95.0, /*minute=*/90);
  const double tired_decay = footbsim::StaminaDecayFactor(/*baseStamina=*/40.0, /*minute=*/90);

  FOOTBSIM_CHECK(tired_decay < fit_decay);
}

FOOTBSIM_TEST(home_advantage_multiplier_increases_effective_attack)
{
  TeamStats stats;
  stats.attack = 60;
  stats.form = 0;

  const double neutral = footbsim::EffectiveAttack(stats, 1.0, /*homeFactor=*/1.0, 1.0, 0.0);
  const double boosted = footbsim::EffectiveAttack(stats, 1.0, /*homeFactor=*/1.15, 1.0, 0.0);

  FOOTBSIM_CHECK(boosted > neutral);
}

FOOTBSIM_TEST(weather_penalty_reduces_effective_attack_but_not_below_floor)
{
  TeamStats stats;
  stats.attack = 50;
  stats.form = 0;

  const double clear = footbsim::EffectiveAttack(stats, 1.0, 1.0, /*weatherFactor=*/1.0, 0.0);
  const double stormy = footbsim::EffectiveAttack(stats, 1.0, 1.0, /*weatherFactor=*/0.8, 0.0);

  FOOTBSIM_CHECK(stormy < clear);
  FOOTBSIM_CHECK(stormy >= 1.0); // floor enforced by std::max in EffectiveAttack
}
