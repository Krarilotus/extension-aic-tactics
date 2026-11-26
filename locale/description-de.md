AIC Tactics steuert für jede installierte KI Rekrutierung, Angriffe, Überfälle und Belagerungsgeräte. Die Einstellungen gehören in ihre AIC. Neue Rekrutierungs-, Angriffs- und Überfallregeln gelten nur, wenn sie gewählt werden; bei fehlenden Belagerungsfeldern gelten die Modulwerte. Truppenlisten und Obergrenzen bleiben gültig.

### Rekrutierung

- `RecruitProbSortieDefault`, `RecruitProbSortieWeak`, `RecruitProbSortieStrong`: Anteil für Ausfälle je Stärkestufe. Sobald du einen Wert setzt, nutzt diese KI vier Rekrutierungsanteile. Verteidigung, Überfälle, Angriff und Ausfälle müssen in jeder Stufe zusammen 100 ergeben.
- `RecruitConditions`: andere Anteile für bestimmte Situationen. Beispiel: Bei Gefahr zu Hause 70 % Verteidigung, 20 % Angriff und 10 % Ausfälle. Die erste passende Regel gilt für diese Rekrutierung.
- `RecruitPolicy`: optionaler Kompatibilitätsschalter. `Native` behält die ursprüngliche Auswahl; `WeightedRoles` aktiviert die vier Anteile ausdrücklich.
- `DefRecruitComposition`: `PreserveSlots` hält die Anteile aus `DefUnit1..8` ein; `Native` behält die bisherige Auswahl.
- `RecruitInitialDefenseMonths`: So lange warten Überfälle und Hauptarmee auf eine volle Verteidigung. 0 schaltet die Wartezeit aus; ohne Wert gilt der Starttimer des Spiels (6 Monate oder der Wert von Legacy `ai_recruitstate_initialtimer`).

Ein Ausfallanteil aktiviert `WeightedRoles` automatisch. Für Situationsregeln, Verteidigermix und `RecruitInitialDefenseMonths` musst du es ausdrücklich setzen. Rekrutierungsintervalle und Kontingente gelten weiter.

### Angriffe und Gegenschläge

- `AttackTargetPolicy`: `Inherit` nutzt `TargetChoice`. `LowestPopulation`, `FewestTroops`, `LowestCombatPower`, `Random` und `LastAggressor` wählen ein Ziel pro Angriff. Für mehrere Angriffe: `{ "Choice": "LastAggressor", "UntilDefeated": true }`. Bei `LastAggressor` kannst du z. B. `"Provocation": { "LossPower": 200 }` ergänzen. `InheritPerAttack` hält die bisherige Wahl während eines Angriffs fest.
- `AttackTargetCommitment`: bleibt für ältere AICs gültig. In neuen AICs stehen Zielwahl und Dauer zusammen in `AttackTargetPolicy`.
- `AttackPreparation`: `DuringAttack` stellt zu Hause die nächste Welle auf, während die Armee angreift; `Native` behält den bisherigen Ablauf.
- `AttackActivation`: `AfterProvocation` wartet vor Angriffen und Überfällen auf einen ausreichenden feindlichen Angriff; `Immediate` wartet nicht. Defensive Ausfälle bleiben möglich.
- `ProvocationRules`: bleibt für ältere AICs sowie `AfterProvocation` und Bedrohungsregeln bei der Rekrutierung gültig. Bei `LastAggressor` können neue AICs die Werte unter `AttackTargetPolicy.Provocation` setzen.

### Angriffspatrouillen

- `AICTactics_PatrolOrders`: `AllGroups` schickt bei einem Angriff auch die zweite Angriffspatrouillengruppe los; `Native` befehligt nur die erste. Damit sie Truppen hat, braucht sie `AttUnitPatrolGroupsCount` 2.
- `AICTactics_PatrolFormation`: `Wing` hält die weiteren Gruppen in einer Linie neben der ersten; `Mirror` schickt sie auf die andere Seite des Ziel-Bergfrieds; `Routes` umkreist die Zielburg in beide Richtungen und läuft ihre Front ab, wie Smarter Patrols; `Native` lässt sie ihre Schritte selbst wählen.
- `AICTactics_PatrolSpacing` (4–40): Felder zwischen benachbarten Gruppen bei `Wing` und `Mirror`.
- `AICTactics_PatrolExtraGroups` (0–6): zusätzliche Patrouillengruppen während eines Angriffs; danach vereinen sie sich wieder.
- `AICTactics_PatrolMinSize` (1–50): Einheiten, die jede Gruppe beim Teilen behält.
- `AICTactics_PatrolRadius` (4–100): Laufweg von der Zielburg zum Ring, dem `Routes` folgt.
- `AICTactics_SiegeScreen`: `EngineApproach` stellt die Belagerungsschutz-Gruppen vor ihre Geräte, beide Gruppen an verschiedene Geräte; `Native` behält den Sammelpunkt des Spiels.
- `AICTactics_SiegeScreenForward` (0–10): Felder vor den Geräten für die Nahkämpfer der Belagerungsschutz-Gruppen.
- `AICTactics_SiegeScreenExtraGroups` (0–6): zusätzliche Belagerungsschutz-Gruppen, solange die Geräte arbeiten, jede an ihrem Gerät; vor dem Mauersturm vereinen sie sich.

Fehlende Werte nutzen die Moduleinstellungen (nur die erste Gruppe, `Native`, 12 Felder, keine Zusatzgruppen, Gruppen zu 4, Ring 12, Geleitschutz aus, 4 Felder, keine zusätzlichen Belagerungsschutz-Gruppen).

### Jagd auf den Burgherrn

Greift eine KI einen menschlichen Spieler an, dessen Burgherr sich hinter den Mauern eines Verbündeten versteckt, belagert sie diese Burg statt der leeren Heimat. Sobald der Weg zum Burgherrn offen ist, geht ein Teil der Armee direkt auf ihn los.

- `AICTactics_LordHuntRetarget`: `true` greift die Burg mit dem Versteck an; `false` bleibt bei seiner eigenen Burg.
- `AICTactics_LordHuntSiege`: `HidingCastle` oder `HomeCastle`, die Burg mit voller Belagerung, solange die eigene Burg des Burgherrn noch Bewohner und Gebäude hat. Die andere Burg erhält einen Teil der Truppen.
- `AICTactics_LordHuntSplitShare` (0–100): dieser Anteil in Prozent.
- `AICTactics_LordHuntSwitchPercent` (0–100): Während eines Angriffs wechselt die KI die Burg nur, solange dieser Anteil ihrer Armee übrig ist.
- `AICTactics_LordHuntShare` (0–100): Anteil der Armee an der Burg des Burgherrn, der ihn jagt; 0 schaltet die Jagd aus.

Fehlende Werte nutzen die Moduleinstellungen (aus, `HidingCastle`, 50, 80, 0). KI-Angriffe auf KI-Spieler bleiben unverändert.

### Hilfe für Verbündete

KI-Verbündete helfen einander und dir, nicht nur auf Bitte.

- `AICTactics_AllyRelief`: `true` schickt einen Teil der freien Armee zu einem Verbündeten, dessen Burg angegriffen wird, und wenn du um Hilfe bittest; `false` behält die Hilfe des Spiels.
- `AICTactics_AllyArmySharePercent` (0–100): Anteil der freien Angriffsarmee, der hilft; 0 schickt niemanden, auch nicht auf Bitte.
- `AICTactics_AllyReliefMinMonths` (0–12): Monate, die der Entsatz mindestens bleibt, wenn beim Verbündeten wieder Ruhe ist.
- `AICTactics_AllyReliefGates`: `true` öffnet die geschlossenen Tore des Verbündeten für den Entsatz, beim Kommen und Gehen.
- `AICTactics_AllyReliefKeep`: `true` stellt den Entsatz auf den Bergfried des Verbündeten statt an seinen Eingang.
- `AICTactics_AllyGoods`: `true` lässt die KI ihre KI-Verbündeten um Waren und Gold bitten, ihnen geben und dir auf Bitte Gold schicken.
- `AICTactics_AllyGoldKeep` (0–100000): Gold, das sie vor jeder Gabe behält, auch auf deine Bitte.
- `AICTactics_AllyRecruitGoldBuffer` (0–5000): Ist sie schwach oder bedroht, erhält sie Hilfe bis zu ihrem Rekrutierungsgold plus diesem Betrag.
- `AICTactics_AllyGoldSharePercent` (0–100): Anteil ihres Goldes über der Reserve, der einem Warengeschenk beiliegt; 0 legt nichts bei, erbetenes Gold wird trotzdem geschickt.
- `AICTactics_AllyMonthlyGivePercent` (0–100): Anteil jeder Ware, den sie pro Monat abgibt.

Fehlende Werte nutzen die Moduleinstellungen (aus, 75 %, 0 Monate, aus, aus, aus, 1000, 300, 25 %, 30 %). Basiert auf Ally Support von Monsterfish.

### Überfälle

- `RaidTargetPolicy`: `Native` behält bisherige Überfälle; `NearestReachable` wählt nahe erreichbare Gebäude; `Opportunistic` berücksichtigt Schwerpunkt und Gefahr; `RandomNearby` wählt ein erreichbares Gebäude zufällig und räumt danach die Umgebung ab.
- `RaidGroupCount` (1–4) teilt die vorhandenen Überfalltruppen auf; `RaidMinGroupSize` (1–256) lässt zu kleine Gruppen warten oder zusammenrücken.
- `RaidFocus` erlaubt `Any`, `Food`, `Industry`, `HighValue` oder Anteile wie `{ "Food": 60, "Industry": 20 }`; der Rest gilt für beliebige Gebäude. Gilt bei `Opportunistic` und `RandomNearby`. `RaidRiskTolerance` bestimmt die Gefahr (`Low`, `Medium`, `High`); `RaidEnemyScope` das Hauptziel oder jeden Feind.

Die fünf Überfallfelder nach `RaidTargetPolicy` benötigen eine neue Überfallregel. `RaidUnitsBase`, `RaidUnitsRandom`, `RaidUnit1..8` und `RaidRetargetDelay` gelten weiter.

### Belagerung und Ingenieure

- `SafeSiegePlacement` meidet belegte Bauplätze (Modulstandard AUS).
- `ActualSiegeResourcePayment` verlangt Rohstoffe und Gold; Fehlmengen kauft die KI über den normalen Handel (AUS).
- `CoordinatedSiegeHarassment` schickt gesammelte Geräte zu erreichbaren Schusspositionen (AUS). `SiegeHarassMinEngines` ist die angestrebte Mindestzahl (0–20, Standard 3); `HarassingSiegeEnginesMax` bleibt die Gesamtgrenze.
- `LargerSiegeForces` wiederholt den Gerätemix, solange lebende, freie Angriffsingenieure und Bauplätze vorhanden sind (AUS). `SiegeForceMax` ist eine optionale Obergrenze (0–64); 0 überlässt die Zahl den Ingenieuren. `AttMaxEngineers` begrenzt weiterhin Besatzungen.

Sichere Bauplätze gelten bei erweiterten Belagerungsregeln automatisch. Das richtige Zählen zugewiesener Ingenieure ist standardmäßig aus; Fixed Engineers betreut Besatzung und Gerätelebenszyklus.

Ausdrückliche AIC-Belagerungswerte haben Vorrang vor Modulwerten.
