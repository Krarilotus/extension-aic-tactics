Az AIC Tactics minden telepített MI-nél külön szabályozza a toborzást, a támadásokat, a portyákat és az ostromgépeket. A mezőket az adott MI AIC-jében add meg. Az új toborzási, támadási és portyaszabályok választhatók; a kihagyott ostrommezők a modul beállításait követik. A meglévő egységlisták és korlátok változatlanok.

### Toborzás

- `RecruitProbSortieDefault`, `RecruitProbSortieWeak`, `RecruitProbSortieStrong`: a kitörések aránya erősségi állapotonként. Bármelyik megadása bekapcsolja a négyféle toborzási arányt ennél az MI-nél. A védelem, portya, támadás és kitörés összege minden állapotban 100 legyen.
- `RecruitConditions`: eltérő arányok adott helyzetben. Például veszélyben lévő várnál 70% védelem, 20% támadás, 10% kitörés. Az első illeszkedő szabály érvényes az adott toborzásra.
- `RecruitPolicy`: opcionális kompatibilitási kapcsoló. A `Native` megtartja az eredeti választást; a `WeightedRoles` kifejezetten bekapcsolja a négy arányt.
- `DefRecruitComposition`: a `PreserveSlots` megtartja a `DefUnit1..8` arányait; a `Native` a megszokott választást használja.
- `RecruitInitialDefenseMonths`: ennyi ideig vár a portya és a fő sereg, amíg a védelem eléri a létszámát. A 0 kikapcsolja a várakozást; ha nincs megadva, a játék kezdeti időzítője érvényes (6 hónap, vagy a Legacy `ai_recruitstate_initialtimer` értéke).

A kitörési arány automatikusan bekapcsolja a `WeightedRoles` módot. A helyzeti szabályokhoz, a védők összetételéhez és a `RecruitInitialDefenseMonths` értékhez külön meg kell adni. A toborzási időközök és korlátok továbbra is érvényesek.

### Támadás és megtorlás

- `AttackTargetPolicy`: az `Inherit` a `TargetChoice` értékét használja. A `LowestPopulation`, `FewestTroops`, `LowestCombatPower`, `Random` és `LastAggressor` támadásonként egy célpontot választ.
- `AttackTargetCommitment`: a régi AIC-kben továbbra is használható. Új AIC-ben a cél és az időtartam együtt állítható az `AttackTargetPolicy` mezőben: `{ "Choice": "LastAggressor", "UntilDefeated": true }`.
- `AttackPreparation`: a `DuringAttack` otthon készíti elő a következő hullámot, miközben a sereg támad; a `Native` megtartja a korábbi ütemet.
- `AttackActivation`: az `AfterProvocation` kellően erős ellenséges támadásra vár a sereg vagy portya indítása előtt; az `Immediate` nem vár. A védekező kitörések megmaradnak.
- `ProvocationRules`: régi AIC-khez, az `AfterProvocation` beállításhoz és a toborzási fenyegetéshez marad. Új AIC-ben a `LastAggressor` küszöbei az `AttackTargetPolicy.Provocation` alatt adhatók meg.

### Támadó járőrök

- `AICTactics_PatrolOrders`: az `AllGroups` roham közben a második támadó járőrcsoportot is elindítja; a `Native` csak az elsőnek ad parancsot. Ehhez az `AttUnitPatrolGroupsCount` értéke 2 legyen, hogy legyenek egységei.
- `AICTactics_PatrolFormation`: a `Wing` a további csoportokat egy vonalban az első mellett tartja; a `Mirror` a célpont vártornyának túloldalára küldi őket; a `Routes` mindkét irányban körbejárja a célvárat, és oda-vissza járja az előterét, mint a Smarter Patrols; a `Native` hagyja, hogy maguk válasszák meg a lépéseiket.
- `AICTactics_PatrolSpacing` (4–40): mezők a szomszédos csoportok között `Wing` és `Mirror` módban.
- `AICTactics_PatrolExtraGroups` (0–6): támadás közben leváló további járőrcsoportok; a végén újra egyesülnek.
- `AICTactics_PatrolMinSize` (1–50): ennyi egységet tart meg minden csoport a felosztáskor.
- `AICTactics_PatrolRadius` (4–100): a `Routes` által követett kör gyalogos távolsága a célvártól.
- `AICTactics_SiegeScreen`: az `EngineApproach` az ostromvédő csoportokat a gépeik elé állítja, a két csoportot különböző gépekhez; a `Native` a játék gyülekezési pontját tartja.
- `AICTactics_SiegeScreenForward` (0–10): mezők a gépek előtt a közelharcos ostromvédő csoportoknak.
- `AICTactics_SiegeScreenExtraGroups` (0–6): további ostromvédő csoportok, amíg a gépek dolgoznak, mindegyik a saját gépénél; a falroham előtt újra egyesülnek.

A hiányzó értékek a modulbeállításokat követik (csak az első csoport, `Native`, 12 mező, nincs további csoport, 4 fős csoportok, 12-es kör, fedezet ki, 4 mező, nincs további fedezőcsoport).

### A várúr üldözése

Ha egy MI olyan emberi játékost támad, akinek várura egy szövetséges falai mögé húzódik, az MI azt a várat ostromolja az üres otthon helyett. Amint szabad az út a várúrhoz, a sereg egy része egyenesen rá rohan.

- `AICTactics_LordHuntRetarget`: a `true` azt a várat támadja, ahol a várúr rejtőzik; a `false` a saját váránál marad.
- `AICTactics_LordHuntSiege`: `HidingCastle` vagy `HomeCastle`, a teljes ostromot kapó vár, amíg a várúr saját várában még vannak lakók és épületek. A másik vár a csapatok egy részét kapja.
- `AICTactics_LordHuntSplitShare` (0–100): ez a rész százalékban.
- `AICTactics_LordHuntSwitchPercent` (0–100): támadás közben az MI csak akkor vált várat, ha seregének még ennyi része megvan.
- `AICTactics_LordHuntShare` (0–100): a várúr váránál álló sereg rá vadászó része; 0 kikapcsolja.

A hiányzó értékek a modulbeállításokat követik (ki, `HidingCastle`, 50, 80, 0). Az MI-játékosok elleni MI-támadások nem változnak.

### Segítség a szövetségeseknek

Az MI-szövetségesek segítik egymást és téged, nem csak kérésre.

- `AICTactics_AllyRelief`: a `true` a tétlen sereg egy részét elküldi egy szövetségeshez, akinek a várát megtámadták, és akkor is, ha segítséget kérsz; a `false` a játék saját segítségét tartja.
- `AICTactics_AllyArmySharePercent` (0–100): a tétlen támadó sereg elküldött része; 0 senkit sem küld, kérésre sem.
- `AICTactics_AllyReliefMinMonths` (0–12): legalább ennyi hónapig marad a felmentés, ha a szövetségesnél már nyugalom van.
- `AICTactics_AllyReliefGates`: a `true` kinyitja a szövetséges zárt kapuit a felmentésnek, érkezéskor és távozáskor.
- `AICTactics_AllyReliefKeep`: a `true` a felmentést a szövetséges vártornyára állítja a bejárat helyett.
- `AICTactics_AllyGoods`: a `true` engedi, hogy az MI árut és aranyat kérjen MI-szövetségeseitől, adjon nekik, és kérésre aranyat küldjön neked.
- `AICTactics_AllyGoldKeep` (0–100000): ennyi aranyat tart meg minden adomány előtt, kérésedre is.
- `AICTactics_AllyRecruitGoldBuffer` (0–5000): gyengén vagy fenyegetve a toborzási aranyáig plusz ennyiig kap segítséget.
- `AICTactics_AllyGoldSharePercent` (0–100): a tartalék feletti aranyából ennyi rész kerül egy áruajándékhoz; 0 semmit sem tesz hozzá, a kért aranyat így is elküldi.
- `AICTactics_AllyMonthlyGivePercent` (0–100): minden áruból havonta ennyi részt ad oda.

A hiányzó értékek a modulbeállításokat követik (ki, 75%, 0 hónap, ki, ki, ki, 1000, 300, 25%, 30%). Monsterfish Ally Support modulja alapján.

### Portyák

- `RaidTargetPolicy`: a `Native` megtartja a portyákat; a `NearestReachable` közeli elérhető épületet választ; az `Opportunistic` a preferenciát és veszélyt is nézi; a `RandomNearby` véletlen elérhető cél után a közeli épületeket támadja.
- A `RaidGroupCount` (1–4) a meglévő portyázókat osztja fel; a `RaidMinGroupSize` (1–256) a kis csoportokat várakoztatja vagy egyesíti.
- A `RaidFocus` lehet `Any`, `Food`, `Industry`, `HighValue` vagy például `{ "Food": 60, "Industry": 20 }`; a maradék bármely épületre jut. `Opportunistic` és `RandomNearby` mellett használható. A `RaidRiskTolerance` a kockázatot, a `RaidEnemyScope` az ellenség körét adja meg.

A `RaidTargetPolicy` utáni öt mezőhöz új portyaszabály kell. A `RaidUnitsBase`, `RaidUnitsRandom`, `RaidUnit1..8` és `RaidRetargetDelay` továbbra is érvényes.

### Ostrom és mérnökök

- `SafeSiegePlacement`: kerüli a foglalt építési helyeket (modul: KI).
- `ActualSiegeResourcePayment`: nyersanyagot és aranyat kér; a hiányt az MI a szokásos kereskedelemmel szerzi be (KI).
- `CoordinatedSiegeHarassment`: összegyűjtött ostromgépeket küld elérhető lőállásokba (KI). A `SiegeHarassMinEngines` a kívánt minimum (0–20, alapérték 3); a `HarassingSiegeEnginesMax` marad az összes gép korlátja.
- `LargerSiegeForces`: addig ismétli a gépösszetételt, amíg élő, szabad támadó mérnök és megfelelő hely van (KI). A `SiegeForceMax` választható korlát (0–64); 0-nál a mérnökök száma dönt. Az `AttMaxEngineers` továbbra is korlátozza a kezelőket.
- `AICTactics_SiegeTowerCrew`: a falak felé haladó ostromtornyokba ülteti az ostromvédő csapatokat; az Improved Siegeengines megmászható ostromtornyai szükségesek hozzá (KI).

A fejlett ostrombeállítások automatikusan bekapcsolják a biztonságos elhelyezést. A mérnökök szerepkörszámlálása alapból kikapcsolt; a Fixed Engineers a kezelők és gépek életciklusát kezeli.

Az AIC-ben megadott ostromértékek megelőzik a modul alapértékeit.
