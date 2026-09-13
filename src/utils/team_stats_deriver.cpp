#include "footbsim/utils/team_stats_deriver.hpp"

#include <algorithm>
#include <cmath>

namespace footbsim::utils
{

  namespace
  {

    // ---- Derivation weights ---------------------------------------------
    // See team_stats_deriver.hpp and apps/cli/brasileirao_2025.hpp for the
    // methodology writeup and the real-season validation these were tuned
    // against (44.7% -> 48.2% match-result prediction accuracy over all
    // 380 real 2025 Brasileirão fixtures).
    constexpr double ATTACK_SOT_WEIGHT = 0.7;
    constexpr double ATTACK_GOALS_WEIGHT = 0.3;
    constexpr double DEFENSE_SOT_WEIGHT = 0.7;
    constexpr double DEFENSE_GOALS_WEIGHT = 0.3;
    constexpr double MIDFIELD_POSSESSION_WEIGHT = 0.7;
    constexpr double MIDFIELD_PASS_ACCURACY_WEIGHT = 0.3;
    constexpr double DISCIPLINE_CARDS_WEIGHT = 0.6;
    constexpr double DISCIPLINE_FOULS_WEIGHT = 0.4;
    constexpr double DISCIPLINE_RED_CARD_MULTIPLIER = 3.0;
    constexpr double AGGRESSION_SHOTS_WEIGHT = 0.7;
    constexpr double AGGRESSION_CORNERS_WEIGHT = 0.3;

    // Fraction of attack/defense/midfield taken from points-per-game
    // rather than the process-stat composite -- process stats capture how
    // a team played, points capture whether it actually worked.
    constexpr double RESULTS_BLEND_WEIGHT = 0.25;

    constexpr double ZSCORE_SCALE = 8.0; // points on the 0-100 scale per standard deviation
    constexpr double ZSCORE_MIN = 5.0;
    constexpr double ZSCORE_MAX = 95.0;

    double Mean(const std::vector<double>& values)
    {
      double sum = 0.0;
      for (double v : values)
      {
        sum += v;
      }
      return sum / static_cast<double>(values.size());
    }

    double PopulationStdDev(const std::vector<double>& values, double mean)
    {
      double sum_sq = 0.0;
      for (double v : values)
      {
        sum_sq += (v - mean) * (v - mean);
      }
      return std::sqrt(sum_sq / static_cast<double>(values.size()));
    }

    // z-scores `values` across the batch onto 50 +/- ZSCORE_SCALE per
    // standard deviation, clamped to [ZSCORE_MIN, ZSCORE_MAX]. `invert`
    // flips the direction, for metrics where a lower raw value is better
    // (e.g. goals conceded).
    std::vector<double> ZScoreTo100(const std::vector<double>& values, bool invert = false)
    {
      const double mean = Mean(values);
      const double stddev = PopulationStdDev(values, mean);

      std::vector<double> result;
      result.reserve(values.size());
      for (double v : values)
      {
        double z = (stddev == 0.0) ? 0.0 : (v - mean) / stddev;
        if (invert)
        {
          z = -z;
        }
        const double score = 50.0 + z * ZSCORE_SCALE;
        result.push_back(std::clamp(score, ZSCORE_MIN, ZSCORE_MAX));
      }
      return result;
    }

  } // namespace

  std::vector<TeamStats> DeriveTeamStats(const std::vector<TeamRawStats>& teams,
                                          double league_strength_offset)
  {
    const std::size_t n = teams.size();

    std::vector<double> attack_process_raw(n);
    std::vector<double> defense_process_raw(n);
    std::vector<double> midfield_process_raw(n);
    std::vector<double> discipline_raw(n);
    std::vector<double> aggression_raw(n);
    std::vector<double> results_raw(n);

    for (std::size_t i = 0; i < n; ++i)
    {
      const TeamRawStats& t = teams[i];
      const double games = static_cast<double>(std::max(t.matches, 1));

      const double sot_pg = t.shots_on_target / games;
      const double goals_pg = t.goals_for / games;
      const double sot_against_pg = t.shots_on_target_against / games;
      const double goals_against_pg = t.goals_against / games;
      const double yellow_pg = t.yellow_cards / games;
      const double red_pg = t.red_cards / games;
      const double fouls_pg = t.fouls / games;
      const double shots_pg = t.shots / games;
      const double corners_pg = t.corners / games;
      const double points_pg = (3.0 * t.wins + t.draws) / games;

      attack_process_raw[i] = ATTACK_SOT_WEIGHT * sot_pg + ATTACK_GOALS_WEIGHT * goals_pg;
      defense_process_raw[i] =
        DEFENSE_SOT_WEIGHT * sot_against_pg + DEFENSE_GOALS_WEIGHT * goals_against_pg;
      midfield_process_raw[i] = MIDFIELD_POSSESSION_WEIGHT * t.possession_pct +
                                MIDFIELD_PASS_ACCURACY_WEIGHT * t.pass_accuracy_pct;
      discipline_raw[i] =
        DISCIPLINE_CARDS_WEIGHT * (yellow_pg + DISCIPLINE_RED_CARD_MULTIPLIER * red_pg) +
        DISCIPLINE_FOULS_WEIGHT * fouls_pg;
      aggression_raw[i] =
        AGGRESSION_SHOTS_WEIGHT * shots_pg + AGGRESSION_CORNERS_WEIGHT * corners_pg;
      results_raw[i] = points_pg;
    }

    const std::vector<double> attack_process = ZScoreTo100(attack_process_raw);
    const std::vector<double> defense_process = ZScoreTo100(defense_process_raw, /*invert=*/true);
    const std::vector<double> midfield_process = ZScoreTo100(midfield_process_raw);
    const std::vector<double> discipline = ZScoreTo100(discipline_raw, /*invert=*/true);
    const std::vector<double> aggression = ZScoreTo100(aggression_raw);
    const std::vector<double> results = ZScoreTo100(results_raw);

    std::vector<TeamStats> out;
    out.reserve(n);
    for (std::size_t i = 0; i < n; ++i)
    {
      const double attack_blend =
        (1.0 - RESULTS_BLEND_WEIGHT) * attack_process[i] + RESULTS_BLEND_WEIGHT * results[i];
      const double defense_blend =
        (1.0 - RESULTS_BLEND_WEIGHT) * defense_process[i] + RESULTS_BLEND_WEIGHT * results[i];
      const double midfield_blend =
        (1.0 - RESULTS_BLEND_WEIGHT) * midfield_process[i] + RESULTS_BLEND_WEIGHT * results[i];

      TeamStats s;
      s.name = teams[i].name;
      s.city = teams[i].city;
      s.state = teams[i].state;
      s.region = teams[i].region;
      s.attack = std::clamp(attack_blend + league_strength_offset, ZSCORE_MIN, ZSCORE_MAX);
      s.defense = std::clamp(defense_blend + league_strength_offset, ZSCORE_MIN, ZSCORE_MAX);
      s.midfield = std::clamp(midfield_blend + league_strength_offset, ZSCORE_MIN, ZSCORE_MAX);
      s.discipline = discipline[i];
      s.aggression = aggression[i];
      // form, stamina, morale left at TeamStats' neutral defaults.
      out.push_back(s);
    }
    return out;
  }

} // namespace footbsim::utils
