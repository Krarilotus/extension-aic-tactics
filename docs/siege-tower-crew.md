# AI siege tower crews

Base: `at/ports` at `632fe5f` in the AIC Tactics repository. The integration worktree under `%TEMP%/cf-review/AT/aic-tactics` contains Lord Hunt, Smarter Patrols, Ally Support and their review fixes. The main module checkout (`feat/recruitment-policy-core`, `4b27456`) and the 0.0.12 engineer-role test bundle predate that integration. See the attack-behavior investigation's `IMPLEMENTATION.md` for the port history.

Improved Siegeengines 1.0.31 (`rq/improved-siegeengines`, `70cc597`) owns climbing. Its `module/init.lua` reads `towers.passengers` and `towers.ai_passengers` separately, then records successful rider installation in local `state.riders`. The configured options alone cannot establish runtime readiness. In `module/riders.lua`, the player command calls private `BOARDTRIBE` to send troops to a tower. The tower update periodically calls private `AIBOARD` for nearby friendly-to-the-tower-owner ranged candidates of the same owner, independent of siege wave. There are nine seats per tower, tracked in the private `TSLOT` table. The module exposes no supported readiness query, available-seat query, or targeted boarding operation. The prerequisite is an owner-side interface for those operations that coordinates with the existing AI auto-boarder so it cannot consume the reserved seats.

AIC Tactics already recognizes the native siege-defence groups sourced from `AttUnitSiegeDef` in the siege screen (`src/patrols.cpp` and `docs/configuration-attacks.md`). These groups are candidates for crew selection. OpenSHC `126f25c9` maps `AICState::aiCommandSiegeEngineTribes` to Crusader `0x4CFCD0` in `AICState.func.hpp` and `Units::UpdateSiegeTower` to `0x544750` in `Units.func.hpp`. The existing `native-siege-bindings.lua` scans the siege command, group movement and assault batch scheduler. Its command-to-movement call-target check passed. The existing patterns matched once each in the two read-only Reconquista-Test PE images using the mapped-image scanner from `tests/check_siege_bindings.py`. The checker needed an in-memory change to make its local Lua result visible to the Python reporter.

| Existing AOB in native-siege-bindings.lua | Crusader (3bb0a8c1) | Extreme (55648e6b) |
| --- | --- | --- |
| Native siege tribe command | 0x4CFCD0 | 0x4CFF20 |
| Native siege group movement | 0x5263A0 | 0x5267C0 |
| Native assault siege batch scheduler | 0x4D4BEB | 0x4D4F9B |

UNVERIFIED: the exact wall-bound departure transition, wave membership at that transition, and boarding before docking in each executable. The command and scheduler matches identify existing code, while an actual tower departure needs separate evidence. No feature hook or runtime binding was added. Proposed per-character field `AICTactics_SiegeTowerCrew` defaults to true when AIC Tactics is active and applies only with working climbing support. The field and round-robin assignment remain pending the rider interface.

Baseline verification: 130 Python tests passed with AIC Loader fixture `82a9a18`; all 37 tracked Lua files parsed via Lupa. Feature tests and live gameplay were skipped because the integration is blocked.
