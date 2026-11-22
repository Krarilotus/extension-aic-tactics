# Attack patrol configuration

The native game recruits up to two attack patrol groups (`AttUnitPatrolGroupsCount` 2) but orders only the first. The second waits at its staging point for the whole assault. These per-character AIC fields let the second group join in. Leave them out and the AI behaves exactly as before.

| Field | Values | Default | Effect |
| --- | --- | --- | --- |
| `PatrolGroupOrders` | `Native`, `AllGroups` | `Native` | `AllGroups` sends the second native patrol group out whenever the game re-orders its first patrol (attack states 3–5). |
| `PatrolFormation` | `Native`, `Wing`, `Mirror` | `Native` | `Native`: the second group searches its own next step from its leader, like the first. `Wing`: it takes a station beside the first group, across its line of advance. `Mirror`: it takes the first group's position reflected to the other side of the target keep. Requires `AllGroups`. |
| `PatrolSpacing` | 4–40 tiles | 12 | Distance of the `Wing` station from the first group's leader. Requires `Wing`. |

A formation station must be reachable for the second group (the game's own tribe path check). Otherwise the other side is tried, then the native search. A group that has reached its station (within three tiles) gets no new order until the first group moves on; while it is still on its way it is re-ordered at the native patrol cadence, like the first group.

## Example

Add these fields to the `aic` object of a copied personality that recruits patrol troops (`AttUnitPatrol` set, `AttUnitPatrolMax` 16 or more):

```json
"AttUnitPatrolGroupsCount": 2,
"PatrolGroupOrders": "AllGroups",
"PatrolFormation": "Wing",
"PatrolSpacing": 14
```

More fragments are in `examples/attack-behaviour/`. They are examples and test fixtures, not presets.

## Saves, replays and multiplayer

- **Saves.** Patrol duties keep no state of their own (other AIC Tactics policies, and its default-on siege placement and engineer counting switches, can still make a save require the module). The groups are the game's two native patrol slots, which every save already contains. After a reload the next native patrol order finds the second group again; nothing is swept back. These fields do not make a save require AIC Tactics.
  - **Module off:** the second group waits at staging, as in the native game.
  - **Changed settings:** a save loads and follows the new settings from the next order.
- **Replays and multiplayer.** Decisions use only synchronized game state at the native order, in fixed slot order, without random numbers. All peers must use the same values: they are part of AIC Tactics' multiplayer admission check.

## Compatibility

- **Smarter Patrols** replaces the same native patrol dispatch and forces a single patrol group. If it, or another module, already changed the dispatch, a character with `PatrolGroupOrders` is refused at load with a message, and nothing is patched over the other module. Use one of the two for patrols.
- **UCP2-Legacy** attack options touch different code and remain compatible.

Concept credit for split attack patrols: Monsterfish (Smarter Patrols). AIC Tactics uses its own implementation built on the game's native patrol slots.
