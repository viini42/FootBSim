#pragma once

#include "footbsim/events.hpp"

#include <string>

namespace footbsim::messages
{

  // Every player-facing string the engine produces lives here -- the event
  // log's play-by-play descriptions and the event-type label shown next to
  // them -- so the whole thing can be translated by editing one file.
  // Currently in Portuguese (pt-BR).

  // Label shown next to an event in the log (e.g. "[45'] <label> - ...").
  inline std::string EventTypeLabel(EventType type)
  {
    switch (type)
    {
    case EventType::KICKOFF:
      return "Início";
    case EventType::PASS:
      return "Passe";
    case EventType::TURNOVER:
      return "Troca de posse";
    case EventType::SHOT:
      return "Chute";
    case EventType::GOAL:
      return "GOL";
    case EventType::SAVE:
      return "Defesa";
    case EventType::BLOCK:
      return "Bloqueio";
    case EventType::MISS:
      return "Para fora";
    case EventType::CORNER:
      return "Escanteio";
    case EventType::FOUL:
      return "Falta";
    case EventType::YELLOW_CARD:
      return "Cartão Amarelo";
    case EventType::RED_CARD:
      return "Cartão Vermelho";
    case EventType::HALF_TIME:
      return "Intervalo";
    case EventType::FULL_TIME:
      return "Fim de Jogo";
    }
    return "Desconhecido";
  }

  // ---- Neutral match milestones (no team attribution) ---------------------

  inline std::string Kickoff()
  {
    return "Início de partida";
  }
  inline std::string HalfTime()
  {
    return "Intervalo";
  }
  inline std::string FullTime()
  {
    return "Fim de jogo";
  }

  // ---- Team-attributed play-by-play ----------------------------------------
  // Each takes the name of the team the sentence is about.

  inline std::string ConcedeFoul(const std::string& team)
  {
    return team + " comete uma falta";
  }

  inline std::string SentOffSecondYellow(const std::string& team)
  {
    return team + " é expulso (segundo cartão amarelo)";
  }

  inline std::string SentOffStraightRed(const std::string& team)
  {
    return team + " é expulso (cartão vermelho direto)";
  }

  inline std::string Booked(const std::string& team)
  {
    return team + " recebe cartão amarelo";
  }

  inline std::string AdvanceToAttackingThird(const std::string& team)
  {
    return team + " avança ao terço final";
  }

  inline std::string KeepPossessionInMidfield(const std::string& team)
  {
    return team + " mantém a posse no meio-campo";
  }

  inline std::string WinBallBackInMidfield(const std::string& team)
  {
    return team + " recupera a bola no meio-campo";
  }

  inline std::string ClearLines(const std::string& team)
  {
    return team + " afasta o perigo";
  }

  inline std::string WinCorner(const std::string& team)
  {
    return team + " ganha um escanteio";
  }

  inline std::string ProbeForOpening(const std::string& team)
  {
    return team + " busca uma abertura";
  }

  inline std::string TakeShot(const std::string& team)
  {
    return team + " finaliza";
  }

  inline std::string Goal(const std::string& team)
  {
    return "GOL de " + team + "!";
  }

  inline std::string KeeperSave(const std::string& team)
  {
    return team + " faz a defesa";
  }

  inline std::string BlockShot(const std::string& team)
  {
    return team + " bloqueia o chute";
  }

  inline std::string ShotWide(const std::string& team)
  {
    return team + " chuta para fora";
  }

  // ---- Match summary (header, scoreline, stat-table labels) ---------------

  inline std::string Versus()
  {
    return "x";
  }

  inline std::string
  FinalScore(const std::string& homeTeam, int homeGoals, const std::string& awayTeam, int awayGoals)
  {
    return "Resultado final: " + homeTeam + " " + std::to_string(homeGoals) + " - " +
           std::to_string(awayGoals) + " " + awayTeam;
  }

  inline std::string StatColumnHeader()
  {
    return "Estatística";
  }
  inline std::string PossessionLabel()
  {
    return "Posse de bola %";
  }
  inline std::string ShotsLabel()
  {
    return "Chutes";
  }
  inline std::string ShotsOnTargetLabel()
  {
    return "Chutes a gol";
  }
  inline std::string CornersLabel()
  {
    return "Escanteios";
  }
  inline std::string FoulsLabel()
  {
    return "Faltas";
  }
  inline std::string YellowCardsLabel()
  {
    return "Cartões amarelos";
  }
  inline std::string RedCardsLabel()
  {
    return "Cartões vermelhos";
  }

} // namespace footbsim::messages
