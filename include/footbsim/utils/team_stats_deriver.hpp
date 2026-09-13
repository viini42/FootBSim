#pragma once

#include "footbsim/team_stats.hpp"

#include <string>
#include <vector>

namespace footbsim::utils
{

  // Raw, season-total match statistics for one team -- the numbers you'd
  // read off a standings/stats page or aggregate yourself from a match
  // log. All the counting stats (shots, fouls, cards, ...) are SEASON
  // TOTALS, not per-game averages -- DeriveTeamStats() does that division
  // itself. possession_pct and pass_accuracy_pct are the team's AVERAGE
  // percentage across the season (summing percentages wouldn't mean
  // anything), not totals. shots_against/shots_on_target_against are the
  // season totals of shots the team faced (i.e. opponents' shots in this
  // team's matches), needed to derive defense the same way attack is
  // derived from the team's own shots.
  struct TeamRawStats
  {
    std::string name;
    int matches = 0;
    int wins = 0;
    int draws = 0;
    int losses = 0;
    int goals_for = 0;
    int goals_against = 0;
    int shots = 0;
    int shots_on_target = 0;
    int shots_against = 0;
    int shots_on_target_against = 0;
    double possession_pct = 0.0;
    double pass_accuracy_pct = 0.0;
    int fouls = 0;
    int corners = 0;
    int yellow_cards = 0;
    int red_cards = 0;
  };

  // Derives TeamStats::attack/defense/midfield/discipline/aggression for
  // every team in `teams`. This is the same methodology validated against
  // the full 2025 Campeonato Brasileiro season log -- see the writeup at
  // the top of apps/cli/brasileirao_2025.hpp for the reasoning and the
  // prediction-accuracy numbers it was checked against.
  //
  // Every stat is z-scored *across the teams passed in* (50 + 8 standard
  // deviations, clamped to [5,95] -- 50 means "average of this batch"),
  // so meaningful results require a full, comparable set of teams -- e.g.
  // a whole league-season -- not one team on its own (a single team
  // always comes back exactly 50 on everything, since it can only be
  // compared to itself). Don't mix teams from different leagues or
  // seasons into one call: the whole method is a relative ranking within
  // the given batch, not an absolute scale.
  //
  // form, stamina and morale aren't derivable from season aggregates
  // (form and morale are inherently dynamic match-to-match values;
  // stamina reflects matchday fitness) and are left at TeamStats' neutral
  // defaults -- set them yourself afterward if you want non-neutral
  // values.
  //
  // `league_strength_offset` shifts the resulting attack/defense/midfield
  // (re-clamped to [5,95]) by a flat number of points -- use it to place
  // two separately-derived divisions onto one shared absolute scale, e.g.
  // when combining a Campeonato Brasileiro Série A batch with a weaker
  // Série B batch: derive each division with its own call (never mix
  // divisions into one call -- that still defeats the within-batch
  // z-score), passing 0.0 for the reference division and a negative
  // offset for the weaker one, then concatenate the two returned vectors.
  // discipline and aggression are style, not competitive strength, so
  // they're unaffected by the offset.
  std::vector<TeamStats> DeriveTeamStats(const std::vector<TeamRawStats>& teams,
                                          double league_strength_offset = 0.0);

} // namespace footbsim::utils
