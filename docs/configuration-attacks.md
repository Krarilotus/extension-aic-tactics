# Attack patrol configuration

The native game recruits up to two attack patrol groups (`AttUnitPatrolGroupsCount` 2) but orders only the first; the second waits at its staging point for the whole assault. With AIC Tactics active, the second group joins in. Per-character AIC fields change this for one AI; module settings in Customizations are the default for every AI.

| AIC field | Module setting | Values | Default | Effect |
| --- | --- | --- | --- | --- |
| `AICTactics_PatrolOrders` | `patrolOrders` | `AllGroups`, `Native` | `AllGroups` | `AllGroups` orders the second native patrol group whenever the game re-orders its first patrol (attack states 3–5). `Native` keeps the game's single-group order. |
| `AICTactics_PatrolFormation` | `patrolFormation` | `Wing`, `Mirror`, `Native` | `Wing` | `Wing`: a station beside the first group, across its line of advance. `Mirror`: once the first group is within 48 tiles of the target keep, its position reflected to the other side of the keep (measured from the keep entrance); when the first group comes straight from home, one of the keep's flanks. `Native`: the second group searches its own next step from its leader, like the first. |
| `AICTactics_PatrolSpacing` | `patrolSpacing` | 4–40 tiles | 12 | Distance of the `Wing` station from the first group's leader (diagonal approaches use an equivalent shorter step). |

A station must lie on the playable map and be reachable for the second group (the game's own tribe path check). Otherwise the other side is tried, then the native search. A group that has reached its station (within three tiles) gets no new order until the first group moves on; while it is on its way it is re-ordered at the native patrol cadence, like the first group.

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

More fragments are in `examples/attack-behaviour/`. They are examples and test fixtures, not presets.

## Saves, replays and multiplayer

- **Saves.** Patrol duties keep no state of their own: the groups are the game's two native patrol slots, which every save already contains. After a reload the next native patrol order finds the second group again; nothing is swept back. A save loads with changed patrol settings and follows them from the next order.
- **Replays and multiplayer.** Decisions use only synchronized game state at the native order, in fixed slot order, without random numbers. All peers must use the same values: they are part of AIC Tactics' multiplayer admission check.

## Compatibility

- AIC Tactics provides what Smarter Patrols did for attack patrols; using both at once is not supported.
- UCP2-Legacy attack options touch different code and remain compatible.

Concept credit for split attack patrols: Monsterfish (Smarter Patrols). AIC Tactics uses its own implementation built on the game's native patrol slots.
