#include "footbsim/utils/team_stats_deriver.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using footbsim::TeamStats;
using footbsim::utils::DeriveTeamStats;
using footbsim::utils::TeamRawStats;

namespace
{

  constexpr int EXPECTED_COLUMNS = 17;

  std::vector<std::string> SplitCsvLine(const std::string& line)
  {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ','))
    {
      fields.push_back(field);
    }
    return fields;
  }

  std::vector<TeamRawStats> LoadTeams(const std::string& path)
  {
    std::ifstream in(path);
    if (!in)
    {
      throw std::runtime_error("could not open input file: " + path);
    }

    std::vector<TeamRawStats> teams;
    std::string line;
    bool first_line = true;
    int line_number = 0;
    while (std::getline(in, line))
    {
      ++line_number;
      if (line.empty())
      {
        continue;
      }
      if (first_line)
      {
        first_line = false;
        continue; // header row
      }

      const std::vector<std::string> f = SplitCsvLine(line);
      if (static_cast<int>(f.size()) != EXPECTED_COLUMNS)
      {
        throw std::runtime_error("line " + std::to_string(line_number) + ": expected " +
                                 std::to_string(EXPECTED_COLUMNS) + " columns, got " +
                                 std::to_string(f.size()));
      }

      TeamRawStats t;
      t.name = f[0];
      t.matches = std::stoi(f[1]);
      t.wins = std::stoi(f[2]);
      t.draws = std::stoi(f[3]);
      t.losses = std::stoi(f[4]);
      t.goals_for = std::stoi(f[5]);
      t.goals_against = std::stoi(f[6]);
      t.shots = std::stoi(f[7]);
      t.shots_on_target = std::stoi(f[8]);
      t.shots_against = std::stoi(f[9]);
      t.shots_on_target_against = std::stoi(f[10]);
      t.possession_pct = std::stod(f[11]);
      t.pass_accuracy_pct = std::stod(f[12]);
      t.fouls = std::stoi(f[13]);
      t.corners = std::stoi(f[14]);
      t.yellow_cards = std::stoi(f[15]);
      t.red_cards = std::stoi(f[16]);
      teams.push_back(std::move(t));
    }

    return teams;
  }

  std::string CapitalizeWord(const std::string& word)
  {
    const bool is_all_upper =
      std::all_of(word.begin(),
                  word.end(),
                  [](unsigned char c) { return std::isalpha(c) == 0 || std::isupper(c) != 0; });
    if (is_all_upper)
    {
      return word;
    }

    std::string result = word;
    if (!result.empty())
    {
      result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0])));
    }
    for (std::size_t i = 1; i < result.size(); ++i)
    {
      result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
    }
    return result;
  }

  // Strips common accented Latin characters (UTF-8 encoded) and any other
  // non-alphanumeric bytes, keeping spaces as word separators. Not
  // exhaustive -- unrecognized multi-byte characters are dropped rather
  // than crashing.
  std::string StripAccentsAndPunctuation(const std::string& input)
  {
    std::string out;
    for (std::size_t i = 0; i < input.size(); ++i)
    {
      const auto c = static_cast<unsigned char>(input[i]);
      if (c == 0xC3 && i + 1 < input.size())
      {
        const auto next = static_cast<unsigned char>(input[i + 1]);
        switch (next)
        {
        case 0xA1:
        case 0xA3:
        case 0xA2:
        case 0xA0:
          out += 'a';
          break; // á ã â à
        case 0x81:
        case 0x83:
        case 0x82:
        case 0x80:
          out += 'A';
          break; // Á Ã Â À
        case 0xA9:
        case 0xAA:
          out += 'e';
          break; // é ê
        case 0x89:
        case 0x8A:
          out += 'E';
          break; // É Ê
        case 0xAD:
          out += 'i';
          break; // í
        case 0x8D:
          out += 'I';
          break; // Í
        case 0xB3:
        case 0xB4:
        case 0xB5:
          out += 'o';
          break; // ó ô õ
        case 0x93:
        case 0x94:
        case 0x95:
          out += 'O';
          break; // Ó Ô Õ
        case 0xBA:
          out += 'u';
          break; // ú
        case 0x9A:
          out += 'U';
          break; // Ú
        case 0xA7:
          out += 'c';
          break; // ç
        case 0x87:
          out += 'C';
          break; // Ç
        default:
          break; // unmapped, drop it
        }
        ++i; // consumed the second byte too
        continue;
      }
      if (std::isalnum(c) != 0 || c == ' ')
      {
        out += static_cast<char>(c);
      }
    }
    return out;
  }

  // Turns a team name into a PascalCase C++ identifier, e.g.
  // "São Paulo" -> "SaoPaulo", "EC Vitória" -> "ECVitoria".
  std::string MakeIdentifier(const std::string& teamName)
  {
    const std::string cleaned = StripAccentsAndPunctuation(teamName);
    std::istringstream words(cleaned);
    std::string word;
    std::string identifier;
    while (words >> word)
    {
      identifier += CapitalizeWord(word);
    }
    return identifier;
  }

  void PrintUsage()
  {
    std::cerr << "usage: footbsim_team_stats_tool <input.csv>\n\n"
              << "CSV columns (one header row, then one row per team):\n"
              << "  name,matches,wins,draws,losses,goals_for,goals_against,shots,\n"
              << "  shots_on_target,shots_against,shots_on_target_against,\n"
              << "  possession_pct,pass_accuracy_pct,fouls,corners,yellow_cards,red_cards\n\n"
              << "All counting stats are SEASON TOTALS, except possession_pct and\n"
              << "pass_accuracy_pct, which are the team's season AVERAGE percentage.\n"
              << "shots_against/shots_on_target_against are totals faced (i.e. what\n"
              << "opponents managed against this team), not this team's own shots.\n\n"
              << "Put every team you want compared together in one file -- stats are\n"
              << "ranked relative to the other rows in that same file, so a single\n"
              << "team on its own will just come back exactly average on everything.\n\n"
              << "attack/defense/midfield/discipline/aggression are derived; form,\n"
              << "stamina and morale can't be (they're dynamic, not season constants),\n"
              << "so they come back at TeamStats' own defaults (0 / 100 / 50). Edit\n"
              << "those by hand afterward if you want non-neutral values.\n\n"
              << "See apps/team_stats_tool/example_teams.csv for a filled-in example.\n";
  }

} // namespace

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    PrintUsage();
    return 1;
  }

  std::vector<TeamRawStats> teams;
  try
  {
    teams = LoadTeams(argv[1]);
  }
  catch (const std::exception& e)
  {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }

  if (teams.empty())
  {
    std::cerr << "error: no teams found in " << argv[1] << "\n";
    return 1;
  }

  const std::vector<TeamStats> derived = DeriveTeamStats(teams);

  std::cout << std::left << std::setw(20) << "Team" << std::right << std::setw(6) << "ATT"
            << std::setw(6) << "DEF" << std::setw(6) << "MID" << std::setw(6) << "DISC"
            << std::setw(6) << "AGG" << "\n";
  for (const TeamStats& stat : derived)
  {
    std::cout << std::left << std::setw(20) << stat.name << std::right << std::fixed
              << std::setprecision(1) << std::setw(6) << stat.attack << std::setw(6) << stat.defense
              << std::setw(6) << stat.midfield << std::setw(6) << stat.discipline << std::setw(6)
              << stat.aggression << "\n";
  }

  std::cout << "\n// Paste into a roster header:\n\n";
  for (const TeamStats& stat : derived)
  {
    std::cout << "  inline TeamStats " << MakeIdentifier(stat.name) << "()\n";
    std::cout << "  {\n";
    std::cout << "    TeamStats s;\n";
    std::cout << "    s.name = \"" << stat.name << "\";\n";
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "    s.attack = " << stat.attack << ";\n";
    std::cout << "    s.defense = " << stat.defense << ";\n";
    std::cout << "    s.midfield = " << stat.midfield << ";\n";
    std::cout << "    s.discipline = " << stat.discipline << ";\n";
    std::cout << "    s.aggression = " << stat.aggression << ";\n";
    std::cout << "    s.form = " << stat.form << ";\n";
    std::cout << "    s.stamina = " << stat.stamina << ";\n";
    std::cout << "    s.morale = " << stat.morale << ";\n";
    std::cout << "    return s;\n";
    std::cout << "  }\n\n";
  }

  return 0;
}
