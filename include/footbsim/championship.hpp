#pragma once

#include "footbsim/team_config.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace footbsim
{

  struct ChampionshipOptions
  {
    // When set, every match gets its own seed derived from this one, so a
    // whole season replays identically. Unset = a fresh random season.
    std::optional<std::uint64_t> seed;

    // true = every pair meets twice (home and away), like the Brasileirão;
    // false = every pair meets once.
    bool double_round_robin = true;

    // Feed each finished match back into both teams' stats (form/morale, see
    // ApplyMatchResult in post_match.hpp) before the next round is played.
    bool update_stats_after_match = true;
  };

  struct MatchRecord
  {
    int round = 0; // 1-based
    std::string home;
    std::string away;
    int home_goals = 0;
    int away_goals = 0;
  };

  struct StandingsEntry
  {
    std::string team;
    int played = 0;
    int wins = 0;
    int draws = 0;
    int losses = 0;
    int goals_for = 0;
    int goals_against = 0;
    int points = 0;

    int GoalDifference() const { return goals_for - goals_against; }
  };

  // A league season played as a round-robin over a fixed set of teams. Each
  // round every team plays at most once (an odd team count gives one team a
  // bye per round), matches are simulated with MatchEngine using each team's
  // own TeamConfig tactics and a MatchContext derived from their locations,
  // and the table is ranked by points, wins, goal difference, goals scored
  // and finally team name (a stable order, not a real-football tiebreaker).
  class Championship
  {
  public:
    // Throws std::invalid_argument for fewer than two teams.
    explicit Championship(std::vector<TeamConfig> teams, ChampionshipOptions options = {});

    int RoundCount() const;
    int RoundsPlayed() const;
    bool IsFinished() const;

    // Plays the next round and returns its results; empty once finished.
    std::vector<MatchRecord> SimulateNextRound();

    // Plays every remaining round.
    void SimulateSeason();

    // Current table, best team first.
    std::vector<StandingsEntry> Standings() const;

    const std::vector<MatchRecord>& Results() const;

    // Teams as they stand now -- with update_stats_after_match, form and
    // morale reflect the matches played so far.
    const std::vector<TeamConfig>& Teams() const;

  private:
    struct Fixture
    {
      int home = 0; // indices into m_teams
      int away = 0;
    };

    void PlayFixture(const Fixture& fixture, int round, std::vector<MatchRecord>& played);

    std::vector<TeamConfig> m_teams;
    ChampionshipOptions m_options;
    std::vector<std::vector<Fixture>> m_schedule; // one entry per round
    std::vector<StandingsEntry> m_table;          // parallel to m_teams, unsorted
    std::vector<MatchRecord> m_results;
    int m_rounds_played = 0;
    std::uint64_t m_matches_played = 0;
  };

  // Prints the table to stdout, one row per team.
  void PrintStandings(const std::vector<StandingsEntry>& standings);

} // namespace footbsim
