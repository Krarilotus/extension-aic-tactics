# Attack patrol and siege-defence configuration

The native game recruits up to two attack patrol groups (`AttUnitPatrolGroupsCount` 2) but orders only the first; the second waits at its staging point for the whole assault. Its siege-defence groups (`AttUnitSiegeDef`) walk together from one siege engine to the next. With AIC Tactics active, every patrol group joins in, a patrol can split into more groups during an attack, and siege-defence groups stand on the engines' approach. Per-character AIC fields change this for one AI; module settings in Customizations are the default for every AI.

## Fields

| AIC field | Module setting | Values | Default | Effect |
| --- | --- | --- | --- | --- |
| `AICTactics_PatrolOrders` | `patrolOrders` | `AllGroups`, `Native` | `AllGroups` | `AllGroups` orders every patrol group after the first whenever the game re-orders its first patrol (attack states 3–5). `Native` keeps the game's single-group order; the fields below then have no effect. |
| `AICTactics_PatrolFormation` | `patrolFormation` | `Routes`, `Wing`, `Mirror`, `Native` | `Routes` | Where the groups after the first go. See [Formations](#formations). |
| `AICTactics_PatrolSpacing` | `patrolSpacing` | 4–40 tiles | 12 | Distance between neighbouring groups in `Wing` and `Mirror` (diagonal approaches use an equivalent shorter step). |
| `AICTactics_PatrolExtraGroups` | `patrolExtraGroups` | 0–6 | 0 | Extra groups split off the native patrol groups while the army attacks. 0 keeps the AI's own groups. |
| `AICTactics_PatrolMinSize` | `patrolMinSize` | 1–50 units | 4 | A patrol is only split as far as every group keeps this many units. |
| `AICTactics_PatrolRadius` | `patrolRadius` | 4–100 | 12 | Walking distance from the target castle of the ring that `Routes` follow. 12 is the ring of the game's own patrol. |
| `AICTactics_SiegeScreen` | `siegeScreen` | `EngineApproach`, `Native` | `EngineApproach` | `EngineApproach` stations the siege-defence groups in front of their engines while the engines work. `Native` keeps the game's rally. |
| `AICTactics_SiegeScreenForward` | `siegeScreenForward` | 0–10 tiles | 4 | How far in front of the engines, toward the target castle, melee siege-defence groups stand. Ranged groups stay at the engines. |

An absent field uses the module setting; an explicit value changes it for that character only. The split and screen defaults are those Smarter Patrols was tested with.

## Formations

The first patrol group (native slot 15) always keeps the game's own order. The others follow the character's formation, in a fixed order: native slot 16, then the extra groups.

- **`Wing`**: the second group stands `PatrolSpacing` tiles beside the first, across its line of advance, on the side nearer its own leader. Every further group stands the same distance beside the group before it, away from the one before that, so the patrol advances as one line.
- **`Mirror`**: once the first group is within 48 tiles of the target keep, the second takes its position reflected to the other side of the keep (measured from the keep entrance); when the first comes straight from home, one of the keep's flanks. Further groups form two lines, alternately beside the first and beside the second group.
- **`Routes`** (Smarter Patrols' routes): the groups walk a ring of 16 points round the target castle. A point is the first tile outward from the keep, in one of 16 directions, whose walking distance from the castle (the AI's own distance map, as its native patrol uses it) lies between `PatrolRadius` and 6 more. Counting the first group as group 1, group 2 circles the castle one way, group 3 walks back and forth along the five points across the front (the side facing the attacker's own keep), and group 4 circles the other way. Groups 5–7 do the same a quarter circle behind, and so on. A circling group starts at the point nearest to it.
- **`Native`**: each group searches its own next step from its leader, as the first does.

Every station or route point must lie on the playable map and be reachable for that group (the game's own tribe path check), and the game must accept the move.

- **`Wing` and `Mirror`.** Otherwise the other side is tried, then the native search. A group that has reached its station (within three tiles) gets no new order; one still on its way is re-ordered at the native patrol cadence, like the first group.
- **`Routes`: the next point.** A group gets its next point when it arrives within five tiles of the current one. Until then it is not re-ordered, so a valid march is never restarted. If the next point is rejected, the one after it is tried.
- **`Routes`: no progress.** If a group gets no closer for 1,600 ticks, it moves on to the next point. Time spent fighting does not count: an enemy within the range the native siege-defence rally treats as a fight. After a second such stall, or when both points are rejected, the native search takes over for 1,600 ticks. The route then resumes from the point nearest the group.

Each AI update issues at most six path checks and three move orders for these groups together.

## Extra groups

While the AI attacks (states 3–5), at its native patrol order:

- **New groups.** At most one new group is opened per update, with the game's own group allocator. The patrol is split into at most the native groups plus `PatrolExtraGroups`, and only as far as every group keeps `PatrolMinSize` units.
- **Balancing.** Up to 16 units per update move from the largest group into the smallest extra group. While an extra group is below the minimum size, groups are balanced to within one unit; afterwards only when they drift apart by more than the minimum size. The groups' leaders are never moved.
- **Refilling.** Units move only into extra groups. The native groups refill from the game's own recruitment.
- **Surplus groups.** When there are more extra groups than fit (losses, or a changed setting), the smallest goes back first.

**Handing back.**

- **Retreat.** When the AI retreats, its extra groups rejoin the native patrol groups before the game orders those home, up to 64 units at once. Any rest walk home as a group and rejoin at later updates.
- **Other ends.** When the attack leaves states 3–5 any other way (a breach, the lord's death), up to 16 units per update rejoin.
- **How.** Units rejoin through the game's own choice of the smallest patrol group. Nothing else is swept: troops in groups AIC Tactics did not record are left alone.

Extra groups use AIC Tactics' shared detached-group records, at most eight per AI across every duty. The count is capped at 6 so that other duties still find room.

## Siege-defence screen

The game walks both siege-defence groups (slots 190 and 191) round its list of the team's built siege engines, in attack states 3 and 4. A group moves on once every member has reached its point and its leader is not fighting.

With `EngineApproach`:

- **Second group.** It takes the engine half a lap further round the same list, so the two groups cover different engines.
- **Melee groups.** A group whose leader is a melee unit stands `SiegeScreenForward` tiles in front of the engine, toward the target keep, and never more than half way there. Ranged groups stay at the engine.
- **Checks and fallback.** Each station is checked like a patrol station; if it is rejected, the game's own rally point is used.
- **Wall assault.** In state 5 the native wall assault orders both groups as before.

## Example

To keep the game's single patrol order for one AI while the module setting is on:

```json
"AICTactics_PatrolOrders": "Native"
```

To give an AI two wing patrols 14 tiles apart:

```json
"AttUnitPatrolGroupsCount": 2,
"AICTactics_PatrolFormation": "Wing",
"AICTactics_PatrolSpacing": 14
```

To split one patrol of up to 24 units into four groups on routes round the target castle:

```json
"AttUnitPatrolMax": 24,
"AICTactics_PatrolExtraGroups": 3,
"AICTactics_PatrolMinSize": 5
```

More fragments are in `examples/attack-behaviour/`. They are examples and test fixtures, not presets.

## Saves, replays and multiplayer

- **Native slots.** The native patrol and siege-defence slots are in every save; after a reload the next native order finds them again.
- **Extra groups and routes.** Extra groups and route progress (point, direction, last progress tick, stalls) are saved in AIC Tactics' state and validated against the game's groups on load. A reload continues the same routes with the same troops.
- **Changed settings.** A save loads with changed settings and follows them from the next order. Surplus extra groups then rejoin the native groups.
- **Without AIC Tactics.** A save made during an attack with extra groups loads, but those groups stand and fight where they are.
- **Replays and multiplayer.** Decisions use only synchronized game state at the native order, integer geometry and a fixed group order, without random numbers. Route records are part of the replay check. All peers must use the same values: they are part of AIC Tactics' multiplayer admission check.

## Compatibility

- **Smarter Patrols.** AIC Tactics provides what Smarter Patrols did; using both at once is not supported.
- **Native sites.** AIC Tactics redirects the three native calls to the attack patrol dispatcher and the move order inside the native siege-defence rally. Each is verified unchanged before it is redirected; if another module already changed one, AIC Tactics refuses with a message instead of patching over it.
- **Recruitment and AI update untouched.** AIC Tactics does not change how many patrol groups the game recruits into, and adds no hook to the per-AI update.
- **UCP2-Legacy.** Its attack options touch different code and remain compatible.

Smarter Patrols by Monsterfish, moved into AIC Tactics under GPL-3.0 with the author's consent, relayed by Krarilotus on 2026-11-23.
