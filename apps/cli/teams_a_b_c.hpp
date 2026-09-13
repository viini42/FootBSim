#pragma once

#include "footbsim/team_stats.hpp"

namespace footbsim::rosters::bra2025
{

  // TeamStats for all 60 clubs across the three tiers of Brazilian football
  // in 2025 -- Série A (20 clubs, full 380-match season), Série B (20
  // clubs, full 380-match season) and Série C (20 clubs, first-phase-only
  // single round-robin, 190 matches, before the knockout stage the top 8
  // go on to play) -- derived from each division's own match log (per-match
  // shots, shots on target, possession, pass accuracy, fouls, corners,
  // cards and results).
  //
  // Each of attack/defense/midfield blends a process-stat composite (how
  // the team actually played) with a results composite (points per game --
  // whether it actually worked). Both are independently z-scored across
  // the 20 clubs of that division as 50 + 8*z (clamped to [5,95]), then
  // blended 75% process / 25% results. discipline and aggression are style
  // measures, not blended with results, since neither is "quality":
  //
  //   attack_process  = 0.7 * SOT/game + 0.3 * goals/game
  //   defense_process = 0.7 * SOT allowed/game + 0.3 * goals allowed/game (inverted)
  //   midfield_process = 0.7 * possession% + 0.3 * pass accuracy%
  //   results = points per game
  //   attack | defense | midfield = 0.75 * <process> + 0.25 * results
  //
  //   discipline = 0.6 * (yellows + 3*reds)/game + 0.4 * fouls/game (inverted)
  //   aggression = 0.7 * shots/game + 0.3 * corners/game
  //
  // The results blend was added after validating against every one of the
  // 380 real fixtures of the 2025 Série A season with the tuned engine:
  // process stats alone missed finishing quality and game management that
  // results capture, and blending them in raised result-direction
  // prediction accuracy from 44.7% to 48.2% without moving the aggregate
  // goals/cards/etc. targets the engine itself was tuned against.
  //
  // Série A is the reference division (league_offset 0.0); Série B and
  // Série C are each a weaker division, so their attack/defense/midfield
  // are shifted down by a flat league_offset (-8.0 and -16.0
  // respectively, re-clamped to [5,95]) after their own division-only
  // z-score, placing all three divisions on one shared absolute scale --
  // see DeriveTeamStats() in team_stats_deriver.hpp for the mechanism, and
  // apps/team_stats_tool/teams_a_b_c.csv (its league_offset column) for
  // the source data this file was pasted from. discipline and aggression
  // are unaffected by the offset, since they're style, not competitive
  // strength.
  //
  // form, stamina and morale have no season-aggregate equivalent (form and
  // morale are inherently dynamic, match-to-match values; stamina reflects
  // matchday fitness), so every club gets the same neutral baseline:
  // form = 0, stamina = 100, morale = 50.
  //
  // city/state/region are identity/metadata, not gameplay stats -- passed
  // straight through from the source CSV, not derived.

  // Paste into a roster header:

  inline TeamStats Atleticomg()
  {
    TeamStats s;
    s.name = "Atlético-MG";
    s.city = "Belo Horizonte";
    s.state = "MG";
    s.region = "Southeast";
    s.attack = 49.6;
    s.defense = 52.2;
    s.midfield = 52.1;
    s.discipline = 52.8;
    s.aggression = 62.1;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Bahia()
  {
    TeamStats s;
    s.name = "Bahia";
    s.city = "Salvador";
    s.state = "BA";
    s.region = "Northeast";
    s.attack = 52.5;
    s.defense = 54.1;
    s.midfield = 57.6;
    s.discipline = 61.3;
    s.aggression = 53.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Botafogo()
  {
    TeamStats s;
    s.name = "Botafogo";
    s.city = "Rio de Janeiro";
    s.state = "RJ";
    s.region = "Southeast";
    s.attack = 59.5;
    s.defense = 57.8;
    s.midfield = 54.4;
    s.discipline = 46.9;
    s.aggression = 57.8;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Bragantino()
  {
    TeamStats s;
    s.name = "Bragantino";
    s.city = "Bragança Paulista";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 47.6;
    s.defense = 43.1;
    s.midfield = 46.5;
    s.discipline = 45.1;
    s.aggression = 47.5;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats CearaSC()
  {
    TeamStats s;
    s.name = "Ceará SC";
    s.city = "Fortaleza";
    s.state = "CE";
    s.region = "Northeast";
    s.attack = 44.5;
    s.defense = 50.8;
    s.midfield = 37.8;
    s.discipline = 54.5;
    s.aggression = 43.3;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Corinthians()
  {
    TeamStats s;
    s.name = "Corinthians";
    s.city = "São Paulo";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 41.9;
    s.defense = 55.2;
    s.midfield = 55.3;
    s.discipline = 45.6;
    s.aggression = 41.1;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Cruzeiro()
  {
    TeamStats s;
    s.name = "Cruzeiro";
    s.city = "Belo Horizonte";
    s.state = "MG";
    s.region = "Southeast";
    s.attack = 57.9;
    s.defense = 56.2;
    s.midfield = 50.4;
    s.discipline = 43.6;
    s.aggression = 55.3;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats ECVitoria()
  {
    TeamStats s;
    s.name = "EC Vitória";
    s.city = "Salvador";
    s.state = "BA";
    s.region = "Northeast";
    s.attack = 43.4;
    s.defense = 43.7;
    s.midfield = 39.3;
    s.discipline = 37.8;
    s.aggression = 38.9;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Flamengo()
  {
    TeamStats s;
    s.name = "Flamengo";
    s.city = "Rio de Janeiro";
    s.state = "RJ";
    s.region = "Southeast";
    s.attack = 65.8;
    s.defense = 63.7;
    s.midfield = 67.3;
    s.discipline = 54.0;
    s.aggression = 65.5;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Fluminense()
  {
    TeamStats s;
    s.name = "Fluminense";
    s.city = "Rio de Janeiro";
    s.state = "RJ";
    s.region = "Southeast";
    s.attack = 48.1;
    s.defense = 60.4;
    s.midfield = 54.7;
    s.discipline = 61.6;
    s.aggression = 44.5;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Fortaleza()
  {
    TeamStats s;
    s.name = "Fortaleza";
    s.city = "Fortaleza";
    s.state = "CE";
    s.region = "Northeast";
    s.attack = 47.9;
    s.defense = 41.4;
    s.midfield = 41.7;
    s.discipline = 45.3;
    s.aggression = 54.1;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Gremio()
  {
    TeamStats s;
    s.name = "Grêmio";
    s.city = "Porto Alegre";
    s.state = "RS";
    s.region = "South";
    s.attack = 48.9;
    s.defense = 45.5;
    s.midfield = 44.3;
    s.discipline = 52.9;
    s.aggression = 44.4;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Internacional()
  {
    TeamStats s;
    s.name = "Internacional";
    s.city = "Porto Alegre";
    s.state = "RS";
    s.region = "South";
    s.attack = 51.6;
    s.defense = 49.3;
    s.midfield = 49.9;
    s.discipline = 41.3;
    s.aggression = 54.3;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Juventude()
  {
    TeamStats s;
    s.name = "Juventude";
    s.city = "Caxias do Sul";
    s.state = "RS";
    s.region = "South";
    s.attack = 38.8;
    s.defense = 41.7;
    s.midfield = 40.6;
    s.discipline = 33.3;
    s.aggression = 36.5;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Mirassol()
  {
    TeamStats s;
    s.name = "Mirassol";
    s.city = "Mirassol";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 58.3;
    s.defense = 50.5;
    s.midfield = 54.3;
    s.discipline = 67.1;
    s.aggression = 50.6;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Palmeiras()
  {
    TeamStats s;
    s.name = "Palmeiras";
    s.city = "São Paulo";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 63.6;
    s.defense = 59.4;
    s.midfield = 56.4;
    s.discipline = 53.8;
    s.aggression = 62.4;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Santos()
  {
    TeamStats s;
    s.name = "Santos";
    s.city = "Santos";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 47.2;
    s.defense = 46.4;
    s.midfield = 49.6;
    s.discipline = 51.1;
    s.aggression = 54.2;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats SportRecife()
  {
    TeamStats s;
    s.name = "Sport Recife";
    s.city = "Recife";
    s.state = "PE";
    s.region = "Northeast";
    s.attack = 38.0;
    s.defense = 30.5;
    s.midfield = 40.5;
    s.discipline = 50.1;
    s.aggression = 46.5;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats SaoPaulo()
  {
    TeamStats s;
    s.name = "São Paulo";
    s.city = "São Paulo";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 42.1;
    s.defense = 50.4;
    s.midfield = 51.9;
    s.discipline = 46.7;
    s.aggression = 42.1;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats VascoDaGama()
  {
    TeamStats s;
    s.name = "Vasco da Gama";
    s.city = "Rio de Janeiro";
    s.state = "RJ";
    s.region = "Southeast";
    s.attack = 52.7;
    s.defense = 47.6;
    s.midfield = 55.2;
    s.discipline = 55.1;
    s.aggression = 46.1;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Amazonas()
  {
    TeamStats s;
    s.name = "Amazonas";
    s.city = "Manaus";
    s.state = "AM";
    s.region = "North";
    s.attack = 28.0;
    s.defense = 35.0;
    s.midfield = 32.8;
    s.discipline = 43.5;
    s.aggression = 43.3;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Americamg()
  {
    TeamStats s;
    s.name = "América-MG";
    s.city = "Belo Horizonte";
    s.state = "MG";
    s.region = "Southeast";
    s.attack = 44.7;
    s.defense = 44.7;
    s.midfield = 48.0;
    s.discipline = 58.4;
    s.aggression = 50.2;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats AthleticClub()
  {
    TeamStats s;
    s.name = "Athletic Club";
    s.city = "São João del-Rei";
    s.state = "MG";
    s.region = "Southeast";
    s.attack = 36.2;
    s.defense = 36.1;
    s.midfield = 43.0;
    s.discipline = 48.7;
    s.aggression = 36.8;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Athleticopr()
  {
    TeamStats s;
    s.name = "Athletico-PR";
    s.city = "Curitiba";
    s.state = "PR";
    s.region = "South";
    s.attack = 56.7;
    s.defense = 50.7;
    s.midfield = 47.8;
    s.discipline = 51.5;
    s.aggression = 56.5;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Atleticogo()
  {
    TeamStats s;
    s.name = "Atlético-GO";
    s.city = "Goiânia";
    s.state = "GO";
    s.region = "Central-West";
    s.attack = 42.2;
    s.defense = 44.1;
    s.midfield = 43.0;
    s.discipline = 38.7;
    s.aggression = 49.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Avai()
  {
    TeamStats s;
    s.name = "Avaí";
    s.city = "Florianópolis";
    s.state = "SC";
    s.region = "South";
    s.attack = 49.5;
    s.defense = 36.9;
    s.midfield = 39.1;
    s.discipline = 55.2;
    s.aggression = 47.4;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Botafogosp()
  {
    TeamStats s;
    s.name = "Botafogo-SP";
    s.city = "Ribeirão Preto";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 32.3;
    s.defense = 40.1;
    s.midfield = 35.3;
    s.discipline = 56.0;
    s.aggression = 39.1;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats CRB()
  {
    TeamStats s;
    s.name = "CRB";
    s.city = "Maceió";
    s.state = "AL";
    s.region = "Northeast";
    s.attack = 53.5;
    s.defense = 31.9;
    s.midfield = 57.3;
    s.discipline = 59.9;
    s.aggression = 72.1;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Chapecoense()
  {
    TeamStats s;
    s.name = "Chapecoense";
    s.city = "Chapecó";
    s.state = "SC";
    s.region = "South";
    s.attack = 51.6;
    s.defense = 43.6;
    s.midfield = 39.7;
    s.discipline = 60.7;
    s.aggression = 51.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Coritiba()
  {
    TeamStats s;
    s.name = "Coritiba";
    s.city = "Curitiba";
    s.state = "PR";
    s.region = "South";
    s.attack = 38.1;
    s.defense = 53.0;
    s.midfield = 51.5;
    s.discipline = 50.5;
    s.aggression = 44.7;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Criciuma()
  {
    TeamStats s;
    s.name = "Criciúma";
    s.city = "Criciúma";
    s.state = "SC";
    s.region = "South";
    s.attack = 43.7;
    s.defense = 49.8;
    s.midfield = 40.7;
    s.discipline = 40.8;
    s.aggression = 49.7;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Cuiaba()
  {
    TeamStats s;
    s.name = "Cuiabá";
    s.city = "Cuiabá";
    s.state = "MT";
    s.region = "Central-West";
    s.attack = 48.8;
    s.defense = 47.2;
    s.midfield = 41.3;
    s.discipline = 42.6;
    s.aggression = 57.1;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Ferroviaria()
  {
    TeamStats s;
    s.name = "Ferroviária";
    s.city = "Araraquara";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 38.9;
    s.defense = 32.6;
    s.midfield = 39.5;
    s.discipline = 53.3;
    s.aggression = 47.2;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Goias()
  {
    TeamStats s;
    s.name = "Goiás";
    s.city = "Goiânia";
    s.state = "GO";
    s.region = "Central-West";
    s.attack = 39.6;
    s.defense = 50.1;
    s.midfield = 40.2;
    s.discipline = 52.5;
    s.aggression = 53.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Novorizontino()
  {
    TeamStats s;
    s.name = "Novorizontino";
    s.city = "Novo Horizonte";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 42.6;
    s.defense = 44.1;
    s.midfield = 44.8;
    s.discipline = 50.6;
    s.aggression = 56.1;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Operariopr()
  {
    TeamStats s;
    s.name = "Operário-PR";
    s.city = "Ponta Grossa";
    s.state = "PR";
    s.region = "South";
    s.attack = 39.6;
    s.defense = 50.3;
    s.midfield = 47.7;
    s.discipline = 40.7;
    s.aggression = 52.6;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Paysandu()
  {
    TeamStats s;
    s.name = "Paysandu";
    s.city = "Belém";
    s.state = "PA";
    s.region = "North";
    s.attack = 34.6;
    s.defense = 32.0;
    s.midfield = 29.9;
    s.discipline = 53.1;
    s.aggression = 52.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Remo()
  {
    TeamStats s;
    s.name = "Remo";
    s.city = "Belém";
    s.state = "PA";
    s.region = "North";
    s.attack = 43.4;
    s.defense = 32.6;
    s.midfield = 40.0;
    s.discipline = 58.5;
    s.aggression = 41.2;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats VilaNova()
  {
    TeamStats s;
    s.name = "Vila Nova";
    s.city = "Goiânia";
    s.state = "GO";
    s.region = "Central-West";
    s.attack = 38.0;
    s.defense = 45.1;
    s.midfield = 31.1;
    s.discipline = 29.8;
    s.aggression = 41.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats VoltaRedonda()
  {
    TeamStats s;
    s.name = "Volta Redonda";
    s.city = "Volta Redonda";
    s.state = "RJ";
    s.region = "Southeast";
    s.attack = 38.0;
    s.defense = 40.0;
    s.midfield = 47.3;
    s.discipline = 55.2;
    s.aggression = 60.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats ABC()
  {
    TeamStats s;
    s.name = "ABC";
    s.city = "Natal";
    s.state = "RN";
    s.region = "Northeast";
    s.attack = 28.3;
    s.defense = 38.0;
    s.midfield = 22.6;
    s.discipline = 56.0;
    s.aggression = 49.3;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Anapolis()
  {
    TeamStats s;
    s.name = "Anápolis";
    s.city = "Anápolis";
    s.state = "GO";
    s.region = "Central-West";
    s.attack = 25.5;
    s.defense = 31.1;
    s.midfield = 23.2;
    s.discipline = 55.1;
    s.aggression = 40.8;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Botafogopb()
  {
    TeamStats s;
    s.name = "Botafogo-PB";
    s.city = "João Pessoa";
    s.state = "PB";
    s.region = "Northeast";
    s.attack = 38.0;
    s.defense = 21.1;
    s.midfield = 37.6;
    s.discipline = 43.9;
    s.aggression = 57.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Brusque()
  {
    TeamStats s;
    s.name = "Brusque";
    s.city = "Brusque";
    s.state = "SC";
    s.region = "South";
    s.attack = 29.0;
    s.defense = 34.3;
    s.midfield = 32.0;
    s.discipline = 49.2;
    s.aggression = 41.9;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats CSA()
  {
    TeamStats s;
    s.name = "CSA";
    s.city = "Maceió";
    s.state = "AL";
    s.region = "Northeast";
    s.attack = 37.3;
    s.defense = 29.2;
    s.midfield = 40.6;
    s.discipline = 46.2;
    s.aggression = 59.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Caxias()
  {
    TeamStats s;
    s.name = "Caxias";
    s.city = "Caxias do Sul";
    s.state = "RS";
    s.region = "South";
    s.attack = 39.6;
    s.defense = 32.9;
    s.midfield = 40.3;
    s.discipline = 52.5;
    s.aggression = 48.4;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Confianca()
  {
    TeamStats s;
    s.name = "Confiança";
    s.city = "Aracaju";
    s.state = "SE";
    s.region = "Northeast";
    s.attack = 43.6;
    s.defense = 31.9;
    s.midfield = 39.5;
    s.discipline = 54.0;
    s.aggression = 63.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Figueirense()
  {
    TeamStats s;
    s.name = "Figueirense";
    s.city = "Florianópolis";
    s.state = "SC";
    s.region = "South";
    s.attack = 40.1;
    s.defense = 30.7;
    s.midfield = 35.0;
    s.discipline = 43.8;
    s.aggression = 56.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Floresta()
  {
    TeamStats s;
    s.name = "Floresta";
    s.city = "Fortaleza";
    s.state = "CE";
    s.region = "Northeast";
    s.attack = 30.0;
    s.defense = 45.0;
    s.midfield = 26.8;
    s.discipline = 46.9;
    s.aggression = 41.2;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Guarani()
  {
    TeamStats s;
    s.name = "Guarani";
    s.city = "Campinas";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 34.6;
    s.defense = 34.3;
    s.midfield = 42.5;
    s.discipline = 39.9;
    s.aggression = 52.7;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Itabaiana()
  {
    TeamStats s;
    s.name = "Itabaiana";
    s.city = "Itabaiana";
    s.state = "SE";
    s.region = "Northeast";
    s.attack = 27.1;
    s.defense = 41.1;
    s.midfield = 23.7;
    s.discipline = 55.1;
    s.aggression = 38.6;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Ituano()
  {
    TeamStats s;
    s.name = "Ituano";
    s.city = "Itu";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 29.3;
    s.defense = 27.4;
    s.midfield = 28.2;
    s.discipline = 52.7;
    s.aggression = 44.8;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Londrina()
  {
    TeamStats s;
    s.name = "Londrina";
    s.city = "Londrina";
    s.state = "PR";
    s.region = "South";
    s.attack = 36.4;
    s.defense = 38.3;
    s.midfield = 36.3;
    s.discipline = 53.8;
    s.aggression = 52.9;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Maringa()
  {
    TeamStats s;
    s.name = "Maringá";
    s.city = "Maringá";
    s.state = "PR";
    s.region = "South";
    s.attack = 47.8;
    s.defense = 33.8;
    s.midfield = 36.9;
    s.discipline = 28.7;
    s.aggression = 68.4;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Nautico()
  {
    TeamStats s;
    s.name = "Náutico";
    s.city = "Recife";
    s.state = "PE";
    s.region = "Northeast";
    s.attack = 39.7;
    s.defense = 49.8;
    s.midfield = 39.8;
    s.discipline = 51.1;
    s.aggression = 49.7;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats PontePreta()
  {
    TeamStats s;
    s.name = "Ponte Preta";
    s.city = "Campinas";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 38.9;
    s.defense = 38.7;
    s.midfield = 45.3;
    s.discipline = 39.6;
    s.aggression = 50.3;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Retro()
  {
    TeamStats s;
    s.name = "Retrô";
    s.city = "Camaragibe";
    s.state = "PE";
    s.region = "Northeast";
    s.attack = 23.8;
    s.defense = 23.0;
    s.midfield = 27.5;
    s.discipline = 52.6;
    s.aggression = 45.0;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats SaoBernardo()
  {
    TeamStats s;
    s.name = "São Bernardo";
    s.city = "São Bernardo do Campo";
    s.state = "SP";
    s.region = "Southeast";
    s.attack = 40.4;
    s.defense = 40.9;
    s.midfield = 43.8;
    s.discipline = 58.3;
    s.aggression = 56.8;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Tombense()
  {
    TeamStats s;
    s.name = "Tombense";
    s.city = "Tombos";
    s.state = "MG";
    s.region = "Southeast";
    s.attack = 23.8;
    s.defense = 27.2;
    s.midfield = 25.9;
    s.discipline = 67.2;
    s.aggression = 40.5;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

  inline TeamStats Ypiranga()
  {
    TeamStats s;
    s.name = "Ypiranga";
    s.city = "Erechim";
    s.state = "RS";
    s.region = "South";
    s.attack = 26.8;
    s.defense = 31.2;
    s.midfield = 32.5;
    s.discipline = 53.4;
    s.aggression = 43.9;
    s.form = 0.0;
    s.stamina = 100.0;
    s.morale = 50.0;
    return s;
  }

} // namespace footbsim::rosters::bra2025
