AIC Tactics permite ajustar el reclutamiento, los ataques, las incursiones y las máquinas de asedio de cada IA instalada. Pon estos campos en su AIC. Las políticas nuevas de reclutamiento, ataque e incursión son opcionales; los campos de asedio omitidos usan los ajustes del módulo. Siguen vigentes las listas y límites de tropas.

### Reclutamiento

- `RecruitProbSortieDefault`, `RecruitProbSortieWeak`, `RecruitProbSortieStrong`: proporción de salidas por nivel de fuerza. Definir una activa las cuatro proporciones para esa IA. Defensa, incursión, ataque y salida deben sumar 100 en cada nivel.
- `RecruitConditions`: proporciones distintas según la situación. Ejemplo: si el castillo está amenazado, 70 % defensa, 20 % ataque y 10 % salidas. Se aplica la primera regla que coincida.
- `RecruitPolicy`: opción de compatibilidad. `Native` conserva la selección original; `WeightedRoles` activa expresamente las cuatro proporciones.
- `DefRecruitComposition`: `PreserveSlots` respeta las proporciones de `DefUnit1..8`; `Native` conserva la elección habitual.
- `RecruitInitialDefenseMonths`: tiempo durante el que incursiones y ejército principal esperan a completar la defensa. 0 elimina la espera; sin valor, se usa el temporizador inicial del juego (6 meses, o el de Legacy `ai_recruitstate_initialtimer`).

Una proporción de salidas activa `WeightedRoles` automáticamente. Las reglas de situación, la composición defensiva y `RecruitInitialDefenseMonths` requieren activarlo expresamente. Los intervalos y cupos siguen vigentes.

### Ataques y represalias

- `AttackTargetPolicy`: `Inherit` usa `TargetChoice`. `LowestPopulation`, `FewestTroops`, `LowestCombatPower`, `Random` y `LastAggressor` eligen un objetivo por ataque. Para conservarlo entre ataques: `{ "Choice": "LastAggressor", "UntilDefeated": true }`. Se puede añadir `"Provocation": { "LossPower": 200 }` para ajustar la represalia.
- `AttackTargetCommitment`: sigue disponible para AIC antiguos. En los nuevos, elige objetivo y duración juntos con `AttackTargetPolicy`.
- `AttackPreparation`: `DuringAttack` prepara la siguiente oleada en casa mientras el ejército ataca; `Native` mantiene los tiempos habituales.
- `AttackActivation`: `AfterProvocation` espera un ataque enemigo suficiente antes de lanzar ejércitos o incursiones; `Immediate` no espera. Las salidas defensivas siguen disponibles.
- `ProvocationRules`: sigue disponible para AIC antiguos, `AfterProvocation` y la amenaza usada al reclutar. Para `LastAggressor`, un AIC nuevo puede poner los valores en `AttackTargetPolicy.Provocation`.

### Patrullas de ataque

- `AICTactics_PatrolOrders`: `AllGroups` envía también el segundo grupo de patrulla de ataque a un asalto; `Native` solo da órdenes al primero. Necesita `AttUnitPatrolGroupsCount` en 2 para tener tropas.
- `AICTactics_PatrolFormation`: `Wing` mantiene los demás grupos en línea junto al primero; `Mirror` los envía al otro lado de la fortaleza objetivo; `Routes` rodea el castillo objetivo en ambos sentidos y recorre su frente de un lado a otro, como Smarter Patrols; `Native` les deja elegir sus propios pasos.
- `AICTactics_PatrolSpacing` (4–40): casillas entre grupos vecinos en `Wing` y `Mirror`.
- `AICTactics_PatrolExtraGroups` (0–6): grupos de patrulla extra durante un ataque; se reúnen al terminar.
- `AICTactics_PatrolMinSize` (1–50): unidades que conserva cada grupo al dividir la patrulla.
- `AICTactics_PatrolRadius` (4–100): distancia a pie desde el castillo objetivo hasta el anillo que sigue `Routes`.
- `AICTactics_SiegeScreen`: `EngineApproach` coloca los grupos de defensa del asedio delante de sus máquinas, los dos en máquinas distintas; `Native` mantiene el punto de reunión del juego.
- `AICTactics_SiegeScreenForward` (0–10): casillas delante de las máquinas para los grupos cuerpo a cuerpo.
- `AICTactics_SiegeScreenExtraGroups` (0–6): grupos de defensa del asedio extra mientras trabajan las máquinas, cada uno en su máquina; se reúnen antes del asalto a la muralla.

Los valores omitidos usan los ajustes del módulo (solo el primer grupo, `Native`, 12 casillas, sin grupos extra, grupos de 4, anillo 12, escolta desactivada, 4 casillas, sin grupos de escolta extra).

### Caza del señor

Cuando una IA ataca a un jugador humano cuyo señor se refugia tras los muros de un aliado, asedia ese castillo en lugar del hogar vacío. En cuanto tiene el camino abierto hasta el señor, parte del ejército va directo a por él.

- `AICTactics_LordHuntRetarget`: `true` ataca el castillo donde se oculta el señor; `false` mantiene el ataque sobre su propio castillo.
- `AICTactics_LordHuntSiege`: `HidingCastle` o `HomeCastle`, el castillo que recibe el asedio completo mientras el del señor aún tenga gente y edificios. El otro recibe parte de las tropas.
- `AICTactics_LordHuntSplitShare` (0–100): esa parte en porcentaje.
- `AICTactics_LordHuntSwitchPercent` (0–100): durante un ataque, la IA solo cambia de castillo mientras le quede esta parte del ejército.
- `AICTactics_LordHuntShare` (0–100): parte del ejército en el castillo del señor que lo caza; 0 desactiva la caza.

Los valores omitidos usan los ajustes del módulo (desactivado, `HidingCastle`, 50, 80, 0). Los ataques de IA contra jugadores IA no cambian.

### Ayuda a aliados

Los aliados IA se ayudan entre sí y a ti, no solo cuando lo pides.

- `AICTactics_AllyRelief`: `true` envía parte del ejército inactivo a un aliado cuyo castillo sufre un ataque, y cuando pides ayuda; `false` mantiene la ayuda del juego.
- `AICTactics_AllyArmySharePercent` (0–100): parte del ejército de ataque inactivo enviada; 0 no envía nada, ni siquiera si lo pides.
- `AICTactics_AllyReliefMinMonths` (0–12): meses que el socorro se queda como mínimo cuando el aliado ya está tranquilo.
- `AICTactics_AllyReliefGates`: `true` abre al socorro las puertas cerradas del aliado, al entrar y al salir.
- `AICTactics_AllyReliefKeep`: `true` coloca el socorro en la fortaleza del aliado en lugar de en su entrada.
- `AICTactics_AllyGoods`: `true` permite a la IA pedir bienes y oro a sus aliados IA, darles y enviarte oro cuando lo pides.
- `AICTactics_AllyGoldKeep` (0–100000): oro que guarda antes de dar nada, también cuando lo pides.
- `AICTactics_AllyRecruitGoldBuffer` (0–5000): si está débil o amenazada, recibe ayuda hasta su oro de reclutamiento más esta cantidad.
- `AICTactics_AllyGoldSharePercent` (0–100): parte de su oro por encima de la reserva que añade a un regalo de bienes; 0 no añade nada, el oro pedido se envía igual.
- `AICTactics_AllyMonthlyGivePercent` (0–100): parte de cada bien que regala al mes.

Los valores omitidos usan los ajustes del módulo (desactivado, 75 %, 0 meses, desactivado, desactivado, desactivado, 1000, 300, 25 %, 30 %). Basado en Ally Support de Monsterfish.

### Incursiones

- `RaidTargetPolicy`: `Native` conserva las incursiones; `NearestReachable` elige edificios cercanos accesibles; `Opportunistic` considera preferencias y peligro; `RandomNearby` elige uno accesible al azar y después ataca los edificios cercanos.
- `RaidGroupCount` (1–4) divide la fuerza existente; `RaidMinGroupSize` (1–256) hace esperar o reunirse a los grupos pequeños.
- `RaidFocus` acepta `Any`, `Food`, `Industry`, `HighValue` o porcentajes como `{ "Food": 60, "Industry": 20 }`; el resto permite cualquier edificio. Funciona con `Opportunistic` y `RandomNearby`. `RaidRiskTolerance` fija el riesgo; `RaidEnemyScope` elige al rival principal o a cualquier enemigo.

Los cinco campos posteriores a `RaidTargetPolicy` requieren una política nueva. `RaidUnitsBase`, `RaidUnitsRandom`, `RaidUnit1..8` y `RaidRetargetDelay` siguen vigentes.

### Asedio e ingenieros

- `SafeSiegePlacement` evita lugares de construcción ocupados (módulo: DESACTIVADO).
- `ActualSiegeResourcePayment` exige materiales y oro; la IA compra lo que falte mediante su comercio normal (DESACTIVADO).
- `CoordinatedSiegeHarassment` envía un grupo de máquinas a posiciones de tiro accesibles (DESACTIVADO). `SiegeHarassMinEngines` fija el mínimo deseado (0–20, valor 3); `HarassingSiegeEnginesMax` sigue limitando el total.
- `LargerSiegeForces` repite la composición mientras queden ingenieros de ataque vivos y libres y sitios válidos (DESACTIVADO). `SiegeForceMax` es un límite opcional (0–64); con 0 decide la cantidad de ingenieros. `AttMaxEngineers` limita las dotaciones.
- `AICTactics_SiegeTowerCrew` sube las tropas de defensa del asedio a las torres de asedio que avanzan hacia la muralla; requiere torres escalables de Improved Siegeengines (DESACTIVADO).

El emplazamiento seguro se activa con las opciones avanzadas de asedio. El recuento de funciones de ingeniero viene desactivado; Fixed Engineers gestiona la vida de las dotaciones.

Los valores de asedio del AIC prevalecen sobre los del módulo.
