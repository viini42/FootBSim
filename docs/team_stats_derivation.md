# Team stats derivation

How `DeriveTeamStats` (`include/footbsim/utils/team_stats_deriver.hpp`) turns
season totals into TeamStats. The method was validated against the full
380-match log of the 2025 Campeonato Brasileiro Série A (per-match shots, shots
on target, possession, pass accuracy, fouls, corners, cards and results).

Each of attack/defense/midfield blends a process-stat composite (how the team
actually played) with a results composite (points per game -- whether it
actually worked). Both are independently z-scored across the teams as
50 + 8*z (clamped to [5,95]), then blended 75% process / 25% results.
discipline and aggression are style measures, not blended with results, since
neither is "quality":

```
attack_process   = 0.7 * SOT/game + 0.3 * goals/game
defense_process  = 0.7 * SOT allowed/game + 0.3 * goals allowed/game (inverted)
midfield_process = 0.7 * possession% + 0.3 * pass accuracy%
results          = points per game
attack | defense | midfield = 0.75 * <process> + 0.25 * results

discipline = 0.6 * (yellows + 3*reds)/game + 0.4 * fouls/game (inverted)
aggression = 0.7 * shots/game + 0.3 * corners/game
```

The results blend was added after validating against every one of the 380 real
fixtures with the tuned engine: process stats alone missed finishing quality and
game management that results capture, and blending them in raised
result-direction prediction accuracy from 44.7% to 48.2% without moving the
aggregate goals/cards/etc. targets the engine itself was tuned against.

form, stamina and morale have no season-aggregate equivalent (form and morale
are inherently dynamic, match-to-match values; stamina reflects matchday
fitness), so they are not derived and come back at neutral defaults.
