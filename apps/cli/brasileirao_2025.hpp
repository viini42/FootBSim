#pragma once

#include "footbsim/team_stats.hpp"

namespace footbsim::rosters::bra2025
{

  // TeamStats for all 20 Campeonato Brasileiro Série A 2025 clubs, derived
  // from the full 380-match log (per-match shots, shots on target,
  // possession, pass accuracy, fouls, corners and cards).
  //
  // Mapping (each per-game average, then z-scored across the 20 clubs as
  // 50 + 8*z, clamped to [5,95] -- 50 is a league-average team; scale chosen
  // so a real few-point gap between closely-matched clubs (e.g. the actual
  // #1 and #2 by points) doesn't get stretched into a lopsided stat gap):
  //   attack     = 0.7 * shots on target/game   + 0.3 * goals/game
  //   defense    = 0.7 * SOT allowed/game       + 0.3 * goals allowed/game   (inverted)
  //   midfield   = 0.7 * possession %           + 0.3 * pass accuracy %
  //   discipline = 0.6 * (yellows + 3*reds)/game + 0.4 * fouls/game          (inverted)
  //   aggression = 0.7 * shots/game             + 0.3 * corners/game
  //
  // form, stamina and morale have no season-aggregate equivalent (form and
  // morale are inherently dynamic, match-to-match values; stamina reflects
  // matchday fitness), so every club gets the same neutral baseline:
  // form = 0, stamina = 85, morale = 50.

  inline TeamStats Flamengo()
  {
    TeamStats s;
    s.name = "Flamengo";
    s.attack = 66.1;
    s.defense = 63.3;
    s.midfield = 68.1;
    s.discipline = 54.0;
    s.aggression = 65.5;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Palmeiras()
  {
    TeamStats s;
    s.name = "Palmeiras";
    s.attack = 63.7;
    s.defense = 58.1;
    s.midfield = 54.1;
    s.discipline = 53.8;
    s.aggression = 62.4;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Botafogo()
  {
    TeamStats s;
    s.name = "Botafogo";
    s.attack = 60.6;
    s.defense = 58.3;
    s.midfield = 53.8;
    s.discipline = 46.9;
    s.aggression = 57.8;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Mirassol()
  {
    TeamStats s;
    s.name = "Mirassol";
    s.attack = 58.3;
    s.defense = 47.9;
    s.midfield = 52.9;
    s.discipline = 67.1;
    s.aggression = 50.6;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Cruzeiro()
  {
    TeamStats s;
    s.name = "Cruzeiro";
    s.attack = 57.2;
    s.defense = 54.9;
    s.midfield = 47.2;
    s.discipline = 43.6;
    s.aggression = 55.3;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats VascoDaGama()
  {
    TeamStats s;
    s.name = "Vasco da Gama";
    s.attack = 54.9;
    s.defense = 48.2;
    s.midfield = 58.2;
    s.discipline = 55.1;
    s.aggression = 46.1;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Internacional()
  {
    TeamStats s;
    s.name = "Internacional";
    s.attack = 53.7;
    s.defense = 50.6;
    s.midfield = 51.3;
    s.discipline = 41.3;
    s.aggression = 54.3;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Bahia()
  {
    TeamStats s;
    s.name = "Bahia";
    s.attack = 51.9;
    s.defense = 54.0;
    s.midfield = 58.6;
    s.discipline = 61.3;
    s.aggression = 53.0;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats AtleticoMG()
  {
    TeamStats s;
    s.name = "Atlético-MG";
    s.attack = 50.2;
    s.defense = 53.7;
    s.midfield = 53.6;
    s.discipline = 52.8;
    s.aggression = 62.1;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Gremio()
  {
    TeamStats s;
    s.name = "Grêmio";
    s.attack = 49.1;
    s.defense = 44.5;
    s.midfield = 43.0;
    s.discipline = 52.9;
    s.aggression = 44.4;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Fortaleza()
  {
    TeamStats s;
    s.name = "Fortaleza";
    s.attack = 48.9;
    s.defense = 40.2;
    s.midfield = 40.7;
    s.discipline = 45.3;
    s.aggression = 54.1;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Bragantino()
  {
    TeamStats s;
    s.name = "Bragantino";
    s.attack = 47.5;
    s.defense = 41.6;
    s.midfield = 46.1;
    s.discipline = 45.1;
    s.aggression = 47.5;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Santos()
  {
    TeamStats s;
    s.name = "Santos";
    s.attack = 47.2;
    s.defense = 46.1;
    s.midfield = 50.4;
    s.discipline = 51.1;
    s.aggression = 54.2;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Fluminense()
  {
    TeamStats s;
    s.name = "Fluminense";
    s.attack = 45.3;
    s.defense = 61.7;
    s.midfield = 54.1;
    s.discipline = 61.6;
    s.aggression = 44.5;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats CearaSC()
  {
    TeamStats s;
    s.name = "Ceará SC";
    s.attack = 44.3;
    s.defense = 52.8;
    s.midfield = 35.4;
    s.discipline = 54.5;
    s.aggression = 43.3;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats ECVitoria()
  {
    TeamStats s;
    s.name = "EC Vitória";
    s.attack = 42.5;
    s.defense = 43.0;
    s.midfield = 37.0;
    s.discipline = 37.8;
    s.aggression = 38.9;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats SportRecife()
  {
    TeamStats s;
    s.name = "Sport Recife";
    s.attack = 40.5;
    s.defense = 30.6;
    s.midfield = 43.8;
    s.discipline = 50.1;
    s.aggression = 46.5;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Corinthians()
  {
    TeamStats s;
    s.name = "Corinthians";
    s.attack = 40.2;
    s.defense = 57.8;
    s.midfield = 58.1;
    s.discipline = 45.6;
    s.aggression = 41.1;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats SaoPaulo()
  {
    TeamStats s;
    s.name = "São Paulo";
    s.attack = 39.7;
    s.defense = 50.7;
    s.midfield = 52.8;
    s.discipline = 46.7;
    s.aggression = 42.1;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Juventude()
  {
    TeamStats s;
    s.name = "Juventude";
    s.attack = 38.2;
    s.defense = 42.1;
    s.midfield = 40.6;
    s.discipline = 33.3;
    s.aggression = 36.5;
    s.form = 0.0;
    s.stamina = 85.0;
    s.morale = 50.0;
    return s;
  }

} // namespace footbsim::rosters::bra2025
