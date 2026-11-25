AIC Tactics 可分别调整每个已安装 AI 的招兵、进攻、袭扰和攻城器械。在该 AI 的 AIC 中填写下列字段。新的招兵、进攻和袭扰策略可按需启用；攻城字段省略时采用模块设置。原有兵种列表和数量上限仍然生效。

### 招兵

- `RecruitProbSortieDefault`, `RecruitProbSortieWeak`, `RecruitProbSortieStrong`：各实力状态下的出击比例。设置任一项后，该 AI 使用四项招兵比例；每档的防御、袭扰、进攻和出击比例合计必须为 100。
- `RecruitConditions`：按情况调整比例。例如城堡受威胁时，70% 防御、20% 进攻、10% 出击。每次招兵采用第一条符合条件的规则。
- `RecruitPolicy`：可选的兼容设置。`Native` 沿用原有选择方式；`WeightedRoles` 明确启用四项比例。
- `DefRecruitComposition`：`PreserveSlots` 保留 `DefUnit1..8` 中各兵种的比例；`Native` 沿用原有选择。
- `RecruitInitialDefenseMonths`：防守人数未达配额时，袭扰和主力军等待的时间。设为 0 则不等待；未设置时沿用游戏的初始计时器（6 个月，或 Legacy `ai_recruitstate_initialtimer` 的设定值）。

设置出击比例会自动启用 `WeightedRoles`。情境规则、防守兵种比例和 `RecruitInitialDefenseMonths` 仍需明确设置此模式。原有招兵间隔和配额继续生效。

### 进攻与反击

- `AttackTargetPolicy`：`Inherit` 使用 `TargetChoice`；`LowestPopulation`、`FewestTroops`、`LowestCombatPower`、`Random` 和 `LastAggressor` 各为一次进攻选定一个目标。
- `AttackTargetCommitment`：旧 AIC 仍可使用。新 AIC 可在 `AttackTargetPolicy` 中同时设置选敌方式和持续时间，例如 `{ "Choice": "LastAggressor", "UntilDefeated": true }`。
- `AttackPreparation`：`DuringAttack` 让军队进攻时在城内准备下一波；`Native` 沿用原有节奏。
- `AttackActivation`：`AfterProvocation` 等待达到反击条件的敌方进攻后再派出主力军或袭扰部队；`Immediate` 不等待。防御性出击仍可进行。
- `ProvocationRules`：旧 AIC、`AfterProvocation` 和招兵威胁规则仍可使用。新 AIC 可在 `AttackTargetPolicy.Provocation` 中设置 `LastAggressor` 的门槛。

### 进攻巡逻队

- `AICTactics_PatrolOrders`：`AllGroups` 在进攻时同时派出第二支进攻巡逻队；`Native` 只指挥第一支。第二支需要 `AttUnitPatrolGroupsCount` 为 2 才有兵力。
- `AICTactics_PatrolFormation`：`Wing` 让其余队伍在第一支旁排成一线；`Mirror` 将其派往目标主堡另一侧；`Routes` 沿两个方向绕目标城堡巡逻，并沿正面来回巡逻，与 Smarter Patrols 相同；`Native` 让其自行决定行动。
- `AICTactics_PatrolSpacing`（4–40）：`Wing` 和 `Mirror` 下相邻队伍间的格数。
- `AICTactics_PatrolExtraGroups`（0–6）：进攻时额外分出的巡逻队，进攻结束后重新合并。
- `AICTactics_PatrolMinSize`（1–50）：拆分巡逻队时每队保留的单位数。
- `AICTactics_PatrolRadius`（4–100）：`Routes` 所走环线与目标城堡的步行距离。
- `AICTactics_SiegeScreen`：`EngineApproach` 让攻城护卫队守在器械前方，两队分守不同器械；`Native` 保持游戏原有集结点。
- `AICTactics_SiegeScreenForward`（0–10）：近战攻城护卫队在器械前方的格数。
- `AICTactics_SiegeScreenExtraGroups`（0–6）：器械作业时额外分出的攻城护卫队，各守一台器械，攻打城墙前重新合并。

未设置的值使用模块设置（全部队伍、`Routes`、12 格、无额外队伍、每队 4 人、环线 12、开启攻城护卫、4 格、无额外护卫队）。

### 追击领主

AI 进攻人类玩家时，若其领主躲在盟友城墙内，AI 会围攻那座城堡，而不是空荡的家园。一旦打通通往领主的道路，部分军队会直扑领主。

- `AICTactics_LordHuntRetarget`：`true` 进攻领主藏身的城堡；`false` 继续进攻其本人的城堡。
- `AICTactics_LordHuntSiege`：`HidingCastle` 或 `HomeCastle`，即领主自己的城堡仍有人口和建筑时获得全力围攻的城堡。另一座城堡分到部分部队。
- `AICTactics_LordHuntSplitShare`（0–100）：该比例，按百分比计。
- `AICTactics_LordHuntSwitchPercent`（0–100）：进攻中，只有剩余兵力不低于此比例时 AI 才会更换城堡。
- `AICTactics_LordHuntShare`（0–100）：在领主所在城堡处追击领主的兵力比例；0 关闭追击。

未设置的值使用模块设置（开启、`HidingCastle`、50、80、50）。AI 对 AI 玩家的进攻不变。

### 援助盟友

AI 盟友会互相援助，也会援助你，而不只是在你请求时。

- `AICTactics_AllyRelief`：`true` 在盟友城堡遭到进攻或你请求援助时派出部分空闲军队；`false` 保持游戏原有援助。
- `AICTactics_AllyArmySharePercent`（0–100）：派出的空闲进攻部队比例；0 不派兵，请求时也不派。
- `AICTactics_AllyReliefMinMonths`（0–12）：盟友恢复平静后援军至少停留的月数。
- `AICTactics_AllyReliefGates`：`true` 在援军进出时打开盟友关闭的城门。
- `AICTactics_AllyReliefKeep`：`true` 让援军站在盟友主堡上，而不是入口处。
- `AICTactics_AllyGoods`：`true` 让 AI 向其 AI 盟友索要物资和金币、向其赠送，并在你请求时送你金币。
- `AICTactics_AllyGoldKeep`（0–100000）：赠送前保留的金币，你请求时也一样。
- `AICTactics_AllyRecruitGoldBuffer`（0–5000）：弱小或受威胁时，获得援助直到其招兵金币再加上此数。
- `AICTactics_AllyGoldSharePercent`（0–100）：随物资附赠的、超出保留金币部分的比例；0 不附赠，请求的金币仍会送出。
- `AICTactics_AllyMonthlyGivePercent`（0–100）：每月赠出每种物资的比例。

未设置的值使用模块设置（开启、75%、0 个月、关闭、关闭、开启、1000、300、25%、30%）。基于 Monsterfish 的 Ally Support。

### 袭扰

- `RaidTargetPolicy`：`Native` 沿用原有袭扰；`NearestReachable` 选择附近可到达的建筑；`Opportunistic` 考虑偏好和危险；`RandomNearby` 随机选一个可到达的建筑，再清理周边建筑。
- `RaidGroupCount`（1–4）拆分现有袭扰兵力；`RaidMinGroupSize`（1–256）让人数不足的小队等待或合并。
- `RaidFocus` 可选 `Any`、`Food`、`Industry`、`HighValue`，也可设比例，如 `{ "Food": 60, "Industry": 20 }`；剩余比例不限类别。适用于 `Opportunistic` 和 `RandomNearby`。`RaidRiskTolerance` 设置危险程度；`RaidEnemyScope` 设置敌方范围。

`RaidTargetPolicy` 后的五个字段需要新袭扰策略。原有 `RaidUnitsBase`, `RaidUnitsRandom`、`RaidUnit1..8` 和 `RaidRetargetDelay` 继续生效。

### 攻城与工程兵

- `SafeSiegePlacement` 避开已被占用的建造位置（模块默认开启）。
- `ActualSiegeResourcePayment` 要求材料和金币；AI 通过正常贸易购买缺少的资源（默认关闭）。
- `CoordinatedSiegeHarassment` 集结攻城器械，再前往可到达的射击位置（默认关闭）。`SiegeHarassMinEngines` 设置期望的最少数量（0–20，默认 3）；`HarassingSiegeEnginesMax` 仍限制总数。
- `LargerSiegeForces` 在仍有存活且未分配的进攻工程兵和有效建造地点时循环使用器械配置（默认关闭）。`SiegeForceMax` 可选数量上限（0–64）；0 由可用工程兵决定。`AttMaxEngineers` 仍限制操作人员。

启用高级攻城功能时，安全选址自动生效。工程兵角色计数默认开启；Fixed Engineers 负责器械和操作人员的生命周期。

攻城 AIC 设置优先于模块默认值。
