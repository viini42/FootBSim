#include "footbsim/match_context.hpp"
#include "footbsim/team_stats.hpp"

#include <catch2/catch_test_macros.hpp>

using footbsim::HomeAdvantageLevel;
using footbsim::MatchContext;
using footbsim::RefereeStyle;
using footbsim::TeamStats;
using footbsim::TravelFatigueLevel;

namespace
{

  TeamStats MakeLocatedTeam(std::string city, std::string state, std::string region)
  {
    TeamStats s;
    s.city = std::move(city);
    s.state = std::move(state);
    s.region = std::move(region);
    return s;
  }

} // namespace

TEST_CASE("home advantage NONE applies no boost", "[match_context][home_advantage]")
{
  CHECK(footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::NONE) == 1.0);
}

TEST_CASE("home advantage multiplier increases with level", "[match_context][home_advantage]")
{
  const double none = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::NONE);
  const double slight = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::SLIGHT);
  const double moderate = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::MODERATE);
  const double strong = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::STRONG);
  const double maximum = footbsim::HomeAdvantageMultiplier(HomeAdvantageLevel::MAXIMUM);

  CHECK(none < slight);
  CHECK(slight < moderate);
  CHECK(moderate < strong);
  CHECK(strong < maximum);
}

TEST_CASE("referee style BALANCED applies no adjustment", "[match_context][referee]")
{
  CHECK(footbsim::RefereeStrictnessMultiplier(RefereeStyle::BALANCED) == 1.0);
}

TEST_CASE("referee strictness multiplier increases with style", "[match_context][referee]")
{
  const double lenient = footbsim::RefereeStrictnessMultiplier(RefereeStyle::LENIENT);
  const double balanced = footbsim::RefereeStrictnessMultiplier(RefereeStyle::BALANCED);
  const double strict = footbsim::RefereeStrictnessMultiplier(RefereeStyle::STRICT);
  const double very_strict = footbsim::RefereeStrictnessMultiplier(RefereeStyle::VERY_STRICT);

  CHECK(lenient < balanced);
  CHECK(balanced < strict);
  CHECK(strict < very_strict);
}

TEST_CASE("travel fatigue SAME_CITY applies no penalty", "[match_context][travel_fatigue]")
{
  CHECK(footbsim::TravelFatiguePenalty(TravelFatigueLevel::SAME_CITY) == 0.0);
}

TEST_CASE("travel fatigue penalty increases with distance", "[match_context][travel_fatigue]")
{
  // A different state (a neighboring state, same region) is a shorter trip
  // than a different region (which spans multiple states), so the penalty
  // ordering is state < region here, not alphabetical/declaration order.
  const double same_city = footbsim::TravelFatiguePenalty(TravelFatigueLevel::SAME_CITY);
  const double state = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_STATE);
  const double region = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_REGION);
  const double country = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_COUNTRY);

  CHECK(same_city < state);
  CHECK(state < region);
  CHECK(region < country);
}

TEST_CASE("travel fatigue stays a small nudge even at its largest",
          "[match_context][travel_fatigue]")
{
  // This is meant as light seasoning on the simulation, not a dominant
  // factor -- guard against it creeping up into something that swings
  // matches on its own.
  const double country = footbsim::TravelFatiguePenalty(TravelFatigueLevel::DIFFERENT_COUNTRY);
  CHECK(country <= 15.0);
}

TEST_CASE("MatchContext derives SAME_CITY for a same-city derby",
          "[match_context][travel_fatigue]")
{
  const TeamStats home = MakeLocatedTeam("Rio de Janeiro", "RJ", "Southeast");
  const TeamStats away = MakeLocatedTeam("Rio de Janeiro", "RJ", "Southeast");
  const MatchContext context(home, away);
  CHECK(context.travel_fatigue == TravelFatigueLevel::SAME_CITY);
}

TEST_CASE("MatchContext derives SAME_CITY for a same-state different-city trip",
          "[match_context][travel_fatigue]")
{
  // No dedicated level exists between a same-city derby and crossing into
  // another state, so a shorter intrastate trip is bucketed with SAME_CITY.
  const TeamStats home = MakeLocatedTeam("São Paulo", "SP", "Southeast");
  const TeamStats away = MakeLocatedTeam("Santos", "SP", "Southeast");
  const MatchContext context(home, away);
  CHECK(context.travel_fatigue == TravelFatigueLevel::SAME_CITY);
}

TEST_CASE("MatchContext derives DIFFERENT_STATE for a same-region different-state trip",
          "[match_context][travel_fatigue]")
{
  const TeamStats home = MakeLocatedTeam("Rio de Janeiro", "RJ", "Southeast");
  const TeamStats away = MakeLocatedTeam("São Paulo", "SP", "Southeast");
  const MatchContext context(home, away);
  CHECK(context.travel_fatigue == TravelFatigueLevel::DIFFERENT_STATE);
}

TEST_CASE("MatchContext derives DIFFERENT_REGION for a cross-region trip",
          "[match_context][travel_fatigue]")
{
  const TeamStats home = MakeLocatedTeam("Rio de Janeiro", "RJ", "Southeast");
  const TeamStats away = MakeLocatedTeam("Porto Alegre", "RS", "South");
  const MatchContext context(home, away);
  CHECK(context.travel_fatigue == TravelFatigueLevel::DIFFERENT_REGION);
}

TEST_CASE("MatchContext leaves every other field at its neutral default",
          "[match_context][travel_fatigue]")
{
  const TeamStats home = MakeLocatedTeam("Rio de Janeiro", "RJ", "Southeast");
  const TeamStats away = MakeLocatedTeam("Porto Alegre", "RS", "South");
  const MatchContext context(home, away);

  CHECK(context.home_advantage == HomeAdvantageLevel::MODERATE);
  CHECK(context.weather == footbsim::Weather::CLEAR);
  CHECK(context.weather_severity == 0.0);
  CHECK(context.referee_strictness == RefereeStyle::BALANCED);
  CHECK(context.stakes == footbsim::Stakes::NORMAL);
}
