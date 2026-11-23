# Ally help configuration

In the native game an AI helps only a human ally: it sends its whole attack army when you ask for help, gives goods you ask for, never gold, and asks only you when it is short. With AIC Tactics active, AI allies help each other and you. Per-character AIC fields change this for one AI; module settings in Customizations are the default for every AI.

| AIC field | Module setting | Values | Default | Effect |
| --- | --- | --- | --- | --- |
| `AICTactics_AllyRelief` | `allyRelief` | `true`, `false` | `true` | `true`: relief troops for an ally whose castle is under attack, and when you ask. `false`: the native help (every attack troop when you ask, nothing for AI allies). |
| `AICTactics_AllyArmySharePercent` | `allyArmySharePercent` | 0–100 | 75 | Share of the idle attack army sent, rounded up. `0` sends no troops, also when you ask: an accepted request is dropped. `100` sends the whole idle attack army. |
| `AICTactics_AllyGoods` | `allyGoods` | `true`, `false` | `true` | `true`: the AI asks its AI allies for goods and gold it cannot buy, gives to them, and sends you gold when you ask. `false`: native trade only. |
| `AICTactics_AllyGoldKeep` | `allyGoldKeep` | 0–100000 | 1000 | Gold this AI keeps for its own recruiting before it gives any away. The same floor applies to your gold requests. |
| `AICTactics_AllyRecruitGoldBuffer` | `allyRecruitGoldBuffer` | 0–5000 | 300 | When this AI is weak or threatened, allies help it up to its `RecruitGoldThreshold` plus this much gold. |
| `AICTactics_AllyGoldSharePercent` | `allyGoldSharePercent` | 0–100 | 25 | Share of the gold above `AllyGoldKeep` this AI adds unasked to a goods gift, to a poorer ally. `0` adds none; gold asked for is still sent. |
| `AICTactics_AllyMonthlyGivePercent` | `allyMonthlyGivePercent` | 0–100 | 30 | Share of each good, gold included, this AI gives away per month. `0` gives nothing; `100` limits gifts only by the stock. |

Booleans and numbers are validated when the AIC is loaded; an invalid value rejects the whole update. Allies are players on the same non-zero team.

## Relief

- **When.** An AI helps when an ally's home is threatened: hostile units worth at least 100 native combat-value points within 32 tiles of the ally's keep (the combat census AIC Tactics already keeps, the scale of `ProvocationRules.ThreatPower`). The most threatened ally goes first. The helper must be idle (not attacking, its next wave not ready), not threatened itself, and hold at least its `AttForceSupportAllyThreshold` attack troops. When you ask an AI for help and the game accepts (the native rules: calm and enough troops), that request is served the same way, without needing a threat.
- **Who goes.** The share of the idle attack army: troops in the AI's own attack groups. Fighters go first; tunnellers, engineers, laddermen and siege engines stay home first. Recruits that join while the relief is out keep the share. At most 32 troops move per AI update.
- **Where.** To the ally's keep entrance, or its camp when the keep cannot be reached (closed gates). A relief that reaches neither goes home.
- **Home again.** When no threat has been seen for one month, when the ally is no longer an ally or its lord died, when the helper is threatened, when its own attack begins or its next wave is ready, or after 12 months. A served request ends when you withdraw it or the relief ends; then it is cleared. The troops are ordered home and rejoin their native groups. After a relief the AI waits 3 months before it helps unasked again (not after its own attack).
- **The native help.** While relief is on, the game's own help (every attack troop, every update, with the rest of the AI's update skipped) never runs for that AI.

## Goods and gold

- **Asking.** An AI that wants goods it cannot buy asks its AI allies directly, by the native request rules: never hops, wheat or ale; food only when nearly out of it; just the amount it wants. Weapons and food are asked first; wood, stone and other materials only while it is not threatened. A lord that has lost a mercenary post, barracks, armoury, granary or market gets wood and stone first, threatened or not. Gold it is still short is asked from AI allies, else from human allies through the game's own request.
- **Giving.** A giver must be calm, not weak and better off than the asker (native strength score); the best off is asked first. It sends a good only while it keeps its AIC `MinimumGoodsRequiredAfterTrade` (a weak asker gets up to half, the giver going down to half that minimum), never a good it wants or got from an ally within 3 months, never more than the asker can store before selling, and within `AllyMonthlyGivePercent`. Gold follows `AllyGoldKeep` the same way.
- **Delays.** A filled request crosses the good off the asker's purchase list and sets the native `AIRequestDelay`; a refused ask waits one month.
- **Your gold requests.** An AI ally now answers your gold request on the ally screen: yes while it keeps `AllyGoldKeep` and stays within `AllyMonthlyGivePercent`, and it sends the gold with its goods-sent message. An AI still building its castle plan gives no gold.

## Saves, replays and multiplayer

- **Saves.** Monthly gifts, recently received goods, the most key buildings seen, the next ask, relief pause and calm timers, and which relief serves a request are part of AIC Tactics' saved state. Relief troops are a recorded detached group (the shared group owner). A cold load continues the same month's allowance and the same relief with the same troops; nothing is given twice, lost or swept back.
- **Replays and multiplayer.** Decisions use synchronized game state at each AI's native update, in fixed player and slot order, without random numbers; the recorded state is part of the replay digest. All peers must use the same values: they are part of AIC Tactics' multiplayer admission check, not of save admission.

## Compatibility

- AIC Tactics provides what Ally Support did; using both is not supported.
- AIC Tactics does not hold allied gates open for relief troops. Gate opening belongs to the gate owners (Smarter Gatehouses, Gatehouse Capture Fix); when closed gates make the keep unreachable, relief troops go to the ally's camp instead.
- AI target policies keep their existing deferral of the native help request; relief serves it instead.

Ally help is Ally Support by Monsterfish, under GPL-3.0; see [CREDITS.md](../CREDITS.md).
