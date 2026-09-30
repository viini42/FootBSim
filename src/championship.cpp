#include "footbsim/championship.hpp"

#include "footbsim/match_context.hpp"
#include "footbsim/match_engine.hpp"
#include "footbsim/messages.hpp"
#include "footbsim/post_match.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace footbsim
{

  namespace
  {

    constexpr int BYE = -1;

    // Circle method: one slot stays fixed while the rest rotate, giving n-1
    // rounds (n rounds with a bye when the team count is odd) in which
    // everybody meets everybody once.
    std::vector<int> InitialSlots(int teamCount)
    {
      std::vector<int> slots;
      for (int i = 0; i < teamCount; ++i)
      {
        slots.push_back(i);
      }
      if (teamCount % 2 != 0)
      {
        slots.push_back(BYE);
      }
      return slots;
    }

    void RotateSlots(std::vector<int>& slots)
    {
      std::rotate(slots.begin() + 1, slots.end() - 1, slots.end());
    }

    void UpdateEntry(StandingsEntry& entry, int goalsFor, int goalsAgainst)
    {
      entry.played += 1;
      entry.goals_for += goalsFor;
      entry.goals_against += goalsAgainst;
      switch (OutcomeFor(goalsFor, goalsAgainst))
      {
      case MatchOutcome::WIN:
        entry.wins += 1;
        entry.points += 3;
        break;
      case MatchOutcome::DRAW:
        entry.draws += 1;
        entry.points += 1;
        break;
      case MatchOutcome::LOSS:
        entry.losses += 1;
        break;
      }
    }

    bool RanksAbove(const StandingsEntry& a, const StandingsEntry& b)
    {
      if (a.points != b.points)
      {
        return a.points > b.points;
      }
      if (a.wins != b.wins)
      {
        return a.wins > b.wins;
      }
      if (a.GoalDifference() != b.GoalDifference())
      {
        return a.GoalDifference() > b.GoalDifference();
      }
      if (a.goals_for != b.goals_for)
      {
        return a.goals_for > b.goals_for;
      }
      return a.team < b.team;
    }

    constexpr int NAME_WIDTH = 22;
    constexpr int NUMBER_WIDTH = 4;

    // std::setw counts bytes, so pad by UTF-8 code points instead to keep
    // accented team names ("Grêmio") aligned with the rest of the column.
    std::string PadName(const std::string& name)
    {
      const auto code_points = static_cast<int>(
        std::count_if(name.begin(), name.end(), [](char c) { return (c & 0xC0) != 0x80; }));
      return name +
             std::string(static_cast<std::size_t>(std::max(0, NAME_WIDTH - code_points)), ' ');
    }

    void PrintColumn(int value)
    {
      std::cout << std::right << std::setw(NUMBER_WIDTH) << value;
    }

  } // namespace

  Championship::Championship(std::vector<TeamConfig> teams, ChampionshipOptions options) :
      m_teams{ std::move(teams) }, m_options{ options }
  {
    if (m_teams.size() < 2)
    {
      throw std::invalid_argument{ "a championship needs at least two teams" };
    }

    for (const TeamConfig& team : m_teams)
    {
      m_table.push_back(StandingsEntry{ .team = team.stats.name });
    }

    std::vector<int> slots = InitialSlots(static_cast<int>(m_teams.size()));
    const int half = static_cast<int>(slots.size()) / 2;
    const int legs = static_cast<int>(slots.size()) - 1;
    for (int round = 0; round < legs; ++round)
    {
      std::vector<Fixture> fixtures;
      for (int i = 0; i < half; ++i)
      {
        const int a = slots[static_cast<std::size_t>(i)];
        const int b = slots[slots.size() - 1 - static_cast<std::size_t>(i)];
        if (a == BYE || b == BYE)
        {
          continue;
        }
        // Flip alternate rounds so the fixed slot doesn't host every time.
        const bool flip = (i == 0 && round % 2 != 0);
        fixtures.push_back(flip ? Fixture{ .home = b, .away = a }
                                : Fixture{ .home = a, .away = b });
      }
      m_schedule.push_back(std::move(fixtures));
      RotateSlots(slots);
    }

    if (m_options.double_round_robin)
    {
      // Second leg replays the first with home and away swapped.
      for (int round = 0; round < legs; ++round)
      {
        std::vector<Fixture> reversed;
        for (const Fixture& fixture : m_schedule[static_cast<std::size_t>(round)])
        {
          reversed.push_back(Fixture{ .home = fixture.away, .away = fixture.home });
        }
        m_schedule.push_back(std::move(reversed));
      }
    }
  }

  int Championship::RoundCount() const
  {
    return static_cast<int>(m_schedule.size());
  }

  int Championship::RoundsPlayed() const
  {
    return m_rounds_played;
  }

  bool Championship::IsFinished() const
  {
    return m_rounds_played >= RoundCount();
  }

  void
  Championship::PlayFixture(const Fixture& fixture, int round, std::vector<MatchRecord>& played)
  {
    TeamConfig& home = m_teams[static_cast<std::size_t>(fixture.home)];
    TeamConfig& away = m_teams[static_cast<std::size_t>(fixture.away)];

    std::optional<std::uint64_t> match_seed;
    if (m_options.seed.has_value())
    {
      match_seed = *m_options.seed + m_matches_played;
    }
    ++m_matches_played;

    MatchEngine engine{ home, away, MatchContext{ home.stats, away.stats }, match_seed };
    const MatchResult result = engine.Simulate();

    UpdateEntry(m_table[static_cast<std::size_t>(fixture.home)],
                result.home_goals,
                result.away_goals);
    UpdateEntry(m_table[static_cast<std::size_t>(fixture.away)],
                result.away_goals,
                result.home_goals);

    played.push_back(MatchRecord{ .round = round,
                                  .home = home.stats.name,
                                  .away = away.stats.name,
                                  .home_goals = result.home_goals,
                                  .away_goals = result.away_goals });

    if (m_options.update_stats_after_match)
    {
      ApplyMatchResult(home.stats, away.stats, result);
    }
  }

  std::vector<MatchRecord> Championship::SimulateNextRound()
  {
    std::vector<MatchRecord> played;
    if (IsFinished())
    {
      return played;
    }

    const int round = m_rounds_played + 1;
    for (const Fixture& fixture : m_schedule[static_cast<std::size_t>(m_rounds_played)])
    {
      PlayFixture(fixture, round, played);
    }
    ++m_rounds_played;

    m_results.insert(m_results.end(), played.begin(), played.end());
    return played;
  }

  void Championship::SimulateSeason()
  {
    while (!IsFinished())
    {
      SimulateNextRound();
    }
  }

  std::vector<StandingsEntry> Championship::Standings() const
  {
    std::vector<StandingsEntry> sorted = m_table;
    std::sort(sorted.begin(), sorted.end(), RanksAbove);
    return sorted;
  }

  const std::vector<MatchRecord>& Championship::Results() const
  {
    return m_results;
  }

  const std::vector<TeamConfig>& Championship::Teams() const
  {
    return m_teams;
  }

  void PrintStandings(const std::vector<StandingsEntry>& standings)
  {
    std::cout << std::left << std::setw(4) << messages::PositionLabel()
              << PadName(messages::TeamLabel());
    for (const std::string& label : { messages::PointsLabel(),
                                      messages::PlayedLabel(),
                                      messages::WinsLabel(),
                                      messages::DrawsLabel(),
                                      messages::LossesLabel(),
                                      messages::GoalsForLabel(),
                                      messages::GoalsAgainstLabel(),
                                      messages::GoalDifferenceLabel() })
    {
      std::cout << std::right << std::setw(NUMBER_WIDTH) << label;
    }
    std::cout << "\n";

    int position = 1;
    for (const StandingsEntry& entry : standings)
    {
      std::cout << std::left << std::setw(4) << position++ << PadName(entry.team);
      PrintColumn(entry.points);
      PrintColumn(entry.played);
      PrintColumn(entry.wins);
      PrintColumn(entry.draws);
      PrintColumn(entry.losses);
      PrintColumn(entry.goals_for);
      PrintColumn(entry.goals_against);
      PrintColumn(entry.GoalDifference());
      std::cout << "\n";
    }
  }

} // namespace footbsim
