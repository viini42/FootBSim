#include "footbsim/match_engine.hpp"
#include "footbsim/team_config.hpp"
#include "footbsim/team_stats.hpp"

#include <catch2/catch_test_macros.hpp>

using footbsim::TeamConfig;
using footbsim::TeamStats;

TEST_CASE("higher attack stat yields higher effective attack", "[effective_stats]")
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
                                                    /*aggressionFactor=*/1.0,
                                                    /*moraleNoise=*/0.0);
  const double eff_strong = footbsim::EffectiveAttack(strong, 1.0, 1.0, 1.0, 1.0, 0.0);

  CHECK(eff_strong > eff_weak);
}

TEST_CASE("stamina decay reduces effective stats over the match", "[effective_stats][stamina]")
{
  TeamStats stats;
  stats.attack = 70;
  stats.form = 0;

  const double early = footbsim::StaminaDecayFactor(stats.stamina, /*minute=*/5);
  const double late = footbsim::StaminaDecayFactor(stats.stamina, /*minute=*/90);

  CHECK(late <= early);

  const double eff_early = footbsim::EffectiveAttack(stats, early, 1.0, 1.0, 1.0, 0.0);
  const double eff_late = footbsim::EffectiveAttack(stats, late, 1.0, 1.0, 1.0, 0.0);
  CHECK(eff_late <= eff_early);
}

TEST_CASE("poor stamina teams decay faster than fit teams", "[stamina]")
{
  const double fit_decay = footbsim::StaminaDecayFactor(/*baseStamina=*/95.0, /*minute=*/90);
  const double tired_decay = footbsim::StaminaDecayFactor(/*baseStamina=*/40.0, /*minute=*/90);

  CHECK(tired_decay < fit_decay);
}

TEST_CASE("home advantage multiplier increases effective attack",
          "[effective_stats][home_advantage]")
{
  TeamStats stats;
  stats.attack = 60;
  stats.form = 0;

  const double neutral = footbsim::EffectiveAttack(stats, 1.0, /*homeFactor=*/1.0, 1.0, 1.0, 0.0);
  const double boosted = footbsim::EffectiveAttack(stats, 1.0, /*homeFactor=*/1.15, 1.0, 1.0, 0.0);

  CHECK(boosted > neutral);
}

TEST_CASE("weather penalty reduces effective attack but not below floor",
          "[effective_stats][weather]")
{
  TeamStats stats;
  stats.attack = 50;
  stats.form = 0;

  const double clear = footbsim::EffectiveAttack(stats, 1.0, 1.0, /*weatherFactor=*/1.0, 1.0, 0.0);
  const double stormy = footbsim::EffectiveAttack(stats, 1.0, 1.0, /*weatherFactor=*/0.8, 1.0, 0.0);

  CHECK(stormy < clear);
  CHECK(stormy >= 1.0); // floor enforced by std::max in EffectiveAttack
}

TEST_CASE("aggression trades off attack against defense", "[aggression]")
{
  // Neutral aggression (50) leaves both factors unchanged.
  CHECK(footbsim::AggressionAttackFactor(50.0) == 1.0);
  CHECK(footbsim::AggressionDefenseFactor(50.0) == 1.0);

  // Attacking tactics (aggression -> 100) boost attack but weaken defense.
  CHECK(footbsim::AggressionAttackFactor(100.0) > 1.0);
  CHECK(footbsim::AggressionDefenseFactor(100.0) < 1.0);

  // Defensive tactics (aggression -> 0) do the reverse.
  CHECK(footbsim::AggressionAttackFactor(0.0) < 1.0);
  CHECK(footbsim::AggressionDefenseFactor(0.0) > 1.0);
}

TEST_CASE("aggression factors stay bounded for out-of-range input", "[aggression]")
{
  CHECK(footbsim::AggressionAttackFactor(-50.0) == footbsim::AggressionAttackFactor(0.0));
  CHECK(footbsim::AggressionAttackFactor(200.0) == footbsim::AggressionAttackFactor(100.0));
}

TEST_CASE("aggression pressure factor increases with aggression", "[aggression]")
{
  CHECK(footbsim::AggressionPressureFactor(50.0) == 1.0);
  CHECK(footbsim::AggressionPressureFactor(100.0) > footbsim::AggressionPressureFactor(50.0));
  CHECK(footbsim::AggressionPressureFactor(0.0) < footbsim::AggressionPressureFactor(50.0));
}

TEST_CASE("EffectiveAggression applies the formation offset and clamps to [0,100]",
          "[effective_stats][formation]")
{
  TeamStats stats;
  stats.aggression = 50.0;

  const TeamConfig balanced{ .stats = stats, .formation = footbsim::Formation::FOUR_FOUR_TWO };
  const TeamConfig attacking{ .stats = stats, .formation = footbsim::Formation::FOUR_THREE_THREE };
  const TeamConfig defensive{ .stats = stats, .formation = footbsim::Formation::FIVE_THREE_TWO };

  CHECK(footbsim::EffectiveAggression(balanced) == 50.0);
  CHECK(footbsim::EffectiveAggression(attacking) > 50.0);
  CHECK(footbsim::EffectiveAggression(defensive) < 50.0);

  TeamStats extreme;
  extreme.aggression = 95.0;
  const TeamConfig extreme_attacking{ .stats = extreme,
                                      .formation = footbsim::Formation::FOUR_THREE_THREE };
  CHECK(footbsim::EffectiveAggression(extreme_attacking) == 100.0); // clamped, not 110
}
