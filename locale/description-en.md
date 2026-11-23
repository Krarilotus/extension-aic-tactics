AIC Tactics lets each installed AI choose how it recruits, attacks, raids and uses siege engines. Set these fields in that AI's AIC. Recruitment, attack and raid policies are opt-in; siege fields inherit the module settings when omitted. Existing troop lists and limits still apply.

### Recruitment

- `RecruitProbSortieDefault`, `RecruitProbSortieWeak`, `RecruitProbSortieStrong`: sortie share in each strength state. Setting one enables four-way recruitment for this AI. Defense, raid, attack and sortie shares must total 100 in every state.
- `RecruitConditions`: optional situation-specific shares. For example, when home is threatened, recruit 70% defense, 20% attack and 10% sorties. The first matching rule applies to that recruitment choice.
- `RecruitPolicy`: optional compatibility switch. `Native` keeps the original selection; `WeightedRoles` explicitly enables four-way selection.
- `DefRecruitComposition`: `PreserveSlots` respects the proportions in `DefUnit1..8`; `Native` keeps existing selection.
- `RecruitInitialDefenseMonths`: how long raid and main-army recruitment waits for the defense quota. 0 disables the wait; default 6 months.

Sortie shares enable `WeightedRoles` automatically. Situation rules, defense composition and `RecruitInitialDefenseMonths` require it explicitly. Existing recruitment intervals and quotas still apply.

### Attacks and retaliation

- `AttackTargetPolicy`: `Inherit` uses `TargetChoice`. Choose `LowestPopulation`, `FewestTroops`, `LowestCombatPower`, `Random` or `LastAggressor` to keep one chosen target per attack. To keep it across attacks, use `{ "Choice": "LastAggressor", "UntilDefeated": true }`. That choice can also contain `"Provocation": { "LossPower": 200 }`; omitted thresholds keep their defaults. `InheritPerAttack` locks the original choice for one attack.
- `AttackTargetCommitment`: older AICs can keep using this field. For new AICs, choose the target and its duration together with `AttackTargetPolicy`.
- `AttackPreparation`: `DuringAttack` prepares the next wave at home while an army attacks; `Native` keeps existing timing.
- `AttackActivation`: `AfterProvocation` waits for a qualifying attack before launching armies or raids; `Immediate` does not wait. Defensive sorties remain available.
- `ProvocationRules`: older AICs, `AfterProvocation` and recruitment threat rules can use this shared threshold field. For `LastAggressor`, new AICs can place overrides inside `AttackTargetPolicy.Provocation`.

### Attack patrols

- `AICTactics_PatrolOrders`: `AllGroups` also sends the second attack patrol group into an assault; `Native` orders only the first. It needs `AttUnitPatrolGroupsCount` 2 to have troops.
- `AICTactics_PatrolFormation`: `Wing` keeps the other groups beside the first in a line; `Mirror` sends them to the other side of the target keep; `Routes` circles the target castle both ways and walks back and forth along its front, as Smarter Patrols did; `Native` lets them choose their own steps.
- `AICTactics_PatrolSpacing` (4–40): tiles between neighbouring groups in `Wing` and `Mirror`.
- `AICTactics_PatrolExtraGroups` (0–6): extra patrol groups split off during an attack; they rejoin when it ends.
- `AICTactics_PatrolMinSize` (1–50): units every group keeps when the patrol is split.
- `AICTactics_PatrolRadius` (4–100): walking distance from the target castle of the ring that `Routes` follow.
- `AICTactics_SiegeScreen`: `EngineApproach` stations the siege-defence groups in front of their engines, the two groups at different engines; `Native` keeps the game's rally.
- `AICTactics_SiegeScreenForward` (0–10): tiles in front of the engines for melee siege-defence groups.

Omitted values use the module settings (every group, `Routes`, 12 tiles, no extra groups, groups of 4, ring 12, siege screen on, 4 tiles).

### Lord hunt

When an AI attacks a human player whose lord shelters inside an ally's walls, the AI sieges that castle instead of the empty home. Once it has an open way to the lord, part of the army goes straight for him.

- `AICTactics_LordHuntRetarget`: `true` attacks the castle the lord hides in; `false` keeps the attack on his own castle.
- `AICTactics_LordHuntSiege`: `HidingCastle` or `HomeCastle`, the castle that gets the full siege while the lord's own castle still has people and buildings. The other castle gets a share of the troops.
- `AICTactics_LordHuntSplitShare` (0–100): that share in percent.
- `AICTactics_LordHuntSwitchPercent` (0–100): during an attack the AI only changes castles while this share of its army remains.
- `AICTactics_LordHuntShare` (0–100): share of the army at the lord's castle that hunts him; 0 turns the hunt off.

Omitted values use the module settings (on, `HidingCastle`, 50, 80, 50). AI attacks on AI players are not changed.

### Raids

- `RaidTargetPolicy`: `Native` keeps existing raids; `NearestReachable` picks nearby accessible buildings; `Opportunistic` weighs focus and danger; `RandomNearby` picks a reachable building at random, then clears nearby buildings before drawing again.
- `RaidGroupCount` (1–4) splits the existing raid force; `RaidMinGroupSize` (1–256) makes undersized groups wait or combine.
- `RaidFocus` can be `Any`, `Food`, `Industry` or `HighValue`, or percentages such as `{ "Food": 60, "Industry": 20 }`; the remainder uses any building. It works with `Opportunistic` and `RandomNearby`. `RaidRiskTolerance` sets `Low`, `Medium` or `High` danger tolerance. `RaidEnemyScope` chooses the main target or any enemy.

The five raid controls after `RaidTargetPolicy` require a new raid policy. Existing `RaidUnitsBase`, `RaidUnitsRandom`, `RaidUnit1..8` and `RaidRetargetDelay` still apply.

### Siege and engineers

- `SafeSiegePlacement` avoids occupied building sites (module default ON).
- `ActualSiegeResourcePayment` requires construction materials and gold, buying shortages through normal AI trade (OFF).
- `CoordinatedSiegeHarassment` sends a gathered group of engines to reachable firing positions (OFF). `SiegeHarassMinEngines` sets its preferred minimum (0–20, default 3); `HarassingSiegeEnginesMax` remains the total limit.
- `LargerSiegeForces` cycles the configured assault equipment mix while living, unassigned attack engineers and valid sites remain (OFF). `SiegeForceMax` is an optional cap (0–64); 0 lets available engineers decide. `AttMaxEngineers` still limits crews.

Safe placement is enabled automatically with advanced siege behavior. Engineer role-quota counting is ON by default; Fixed Engineers owns general crew and equipment lifecycle.

Explicit AIC siege settings override module fallbacks.
