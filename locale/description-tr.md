AIC Tactics, kurulu her yapay zekânın asker alımını, saldırılarını, baskınlarını ve kuşatma araçlarını ayarlamanı sağlar. Alanları o yapay zekânın AIC dosyasına ekle. Yeni asker alımı, saldırı ve baskın kuralları isteğe bağlıdır; boş bırakılan kuşatma alanları modül ayarlarını kullanır. Mevcut birlik listeleri ve sınırlar geçerlidir.

### Asker alımı

- `RecruitProbSortieDefault`, `RecruitProbSortieWeak`, `RecruitProbSortieStrong`: her güç durumundaki çıkış payı. Bunlardan birini ayarlamak, bu yapay zekâ için dört asker alım payını etkinleştirir. Savunma, baskın, saldırı ve çıkış payları her durumda toplam 100 olmalıdır.
- `RecruitConditions`: duruma göre farklı paylar. Örneğin kale tehdit altındaysa %70 savunma, %20 saldırı, %10 çıkış. Her asker alımında ilk eşleşen kural uygulanır.
- `RecruitPolicy`: isteğe bağlı uyumluluk ayarı. `Native` özgün seçimi korur; `WeightedRoles` dört payı açıkça etkinleştirir.
- `DefRecruitComposition`: `PreserveSlots`, `DefUnit1..8` oranlarını korur; `Native` önceki seçimi kullanır.
- `RecruitInitialDefenseMonths`: baskın ve ana ordunun savunma kotasını beklediği süre. 0 beklemeyi kapatır; ayarlanmazsa oyunun başlangıç zamanlayıcısı geçerlidir (6 ay ya da Legacy `ai_recruitstate_initialtimer` değeri).

Çıkış payı `WeightedRoles` ayarını otomatik açar. Durum kuralları, savunma dağılımı ve `RecruitInitialDefenseMonths` için bunu açıkça seçmek gerekir. Asker alım aralıkları ve kotaları geçerliliğini korur.

### Saldırılar ve karşılık verme

- `AttackTargetPolicy`: `Inherit`, `TargetChoice` değerini kullanır. `LowestPopulation`, `FewestTroops`, `LowestCombatPower`, `Random` ve `LastAggressor` her saldırı için bir hedef seçer.
- `AttackTargetCommitment`: eski AIC'lerde geçerlidir. Yeni AIC'lerde seçim ve süreyi birlikte `AttackTargetPolicy` içinde yaz: `{ "Choice": "LastAggressor", "UntilDefeated": true }`.
- `AttackPreparation`: `DuringAttack`, ordu saldırırken sonraki dalgayı kalede hazırlar; `Native` önceki zamanlamayı korur.
- `AttackActivation`: `AfterProvocation`, ordu veya baskın başlatmadan önce yeterli düşman saldırısını bekler; `Immediate` beklemez. Savunma çıkışları kullanılabilir.
- `ProvocationRules`: eski AIC'ler, `AfterProvocation` ve asker alımındaki tehdit kuralları için geçerlidir. Yeni AIC'lerde `LastAggressor` eşiklerini `AttackTargetPolicy.Provocation` içinde yazabilirsin.

### Saldırı devriyeleri

- `AICTactics_PatrolOrders`: `AllGroups` hücumda ikinci saldırı devriye grubunu da gönderir; `Native` yalnızca ilkine emir verir. Birlik alabilmesi için `AttUnitPatrolGroupsCount` değerinin 2 olması gerekir.
- `AICTactics_PatrolFormation`: `Wing` diğer grupları ilk grubun yanında bir hatta tutar; `Mirror` onları hedef iç kalenin öbür tarafına gönderir; `Routes`, Smarter Patrols gibi hedef kalenin çevresinde iki yönde döner ve önü boyunca gidip gelir; `Native` kendi adımlarını seçmelerine izin verir.
- `AICTactics_PatrolSpacing` (4–40): `Wing` ve `Mirror` düzeninde komşu gruplar arasındaki kare sayısı.
- `AICTactics_PatrolExtraGroups` (0–6): saldırı sırasında ayrılan ek devriye grupları; saldırı bitince birleşirler.
- `AICTactics_PatrolMinSize` (1–50): devriye bölündüğünde her grubun tuttuğu birim sayısı.
- `AICTactics_PatrolRadius` (4–100): `Routes` tarafından izlenen halkanın hedef kaleye yürüme mesafesi.
- `AICTactics_SiegeScreen`: `EngineApproach` kuşatma savunma gruplarını araçlarının önüne, iki grubu farklı araçlara yerleştirir; `Native` oyunun toplanma noktasını korur.
- `AICTactics_SiegeScreenForward` (0–10): yakın dövüş kuşatma savunma grupları için araçların önündeki kare sayısı.
- `AICTactics_SiegeScreenExtraGroups` (0–6): araçlar çalışırken ayrılan, her biri kendi aracında duran ek kuşatma savunma grupları; sur hücumundan önce birleşirler.

Belirtilmeyen değerler modül ayarlarını kullanır (yalnızca ilk grup, `Native`, 12 kare, ek grup yok, 4'lük gruplar, halka 12, muhafız kapalı, 4 kare, ek muhafız grubu yok).

### Lord avı

Bir yapay zekâ, lordu bir müttefikin surları ardına sığınmış bir insan oyuncuya saldırdığında, boş yurdu yerine o kaleyi kuşatır. Lorda giden yol açılınca ordunun bir kısmı doğrudan ona yönelir.

- `AICTactics_LordHuntRetarget`: `true` lordun saklandığı kaleye saldırır; `false` saldırıyı lordun kendi kalesinde tutar.
- `AICTactics_LordHuntSiege`: `HidingCastle` ya da `HomeCastle`; lordun kendi kalesinde hâlâ halk ve bina varken tam kuşatmayı alan kale. Diğer kale birliklerin bir kısmını alır.
- `AICTactics_LordHuntSplitShare` (0–100): bu pay, yüzde olarak.
- `AICTactics_LordHuntSwitchPercent` (0–100): saldırı sırasında yapay zekâ, yalnızca ordusunun bu payı kaldıkça kale değiştirir.
- `AICTactics_LordHuntShare` (0–100): lordun kalesindeki ordudan onu avlayan pay; 0 avı kapatır.

Belirtilmeyen değerler modül ayarlarını kullanır (kapalı, `HidingCastle`, 50, 80, 0). Yapay zekâ oyunculara yapılan yapay zekâ saldırıları değişmez.

### Müttefik yardımı

Yapay zekâ müttefikleri yalnızca istendiğinde değil, kendiliğinden de birbirine ve sana yardım eder.

- `AICTactics_AllyRelief`: `true` boştaki ordunun bir kısmını kalesi saldırı altındaki bir müttefike ve sen yardım istediğinde gönderir; `false` oyunun kendi yardımını korur.
- `AICTactics_AllyArmySharePercent` (0–100): gönderilen boştaki saldırı ordusu payı; 0 istendiğinde bile göndermez.
- `AICTactics_AllyReliefMinMonths` (0–12): müttefik sakinleştikten sonra takviyenin en az kaldığı ay sayısı.
- `AICTactics_AllyReliefGates`: `true` müttefikin kapalı kapılarını takviyeye girişte ve çıkışta açar.
- `AICTactics_AllyReliefKeep`: `true` takviyeyi müttefikin girişi yerine iç kalesine koyar.
- `AICTactics_AllyGoods`: `true` yapay zekânın yapay zekâ müttefiklerinden mal ve altın istemesine, onlara vermesine ve sen istediğinde sana altın göndermesine izin verir.
- `AICTactics_AllyGoldKeep` (0–100000): bir şey vermeden önce ayırdığı altın, sen istediğinde de.
- `AICTactics_AllyRecruitGoldBuffer` (0–5000): zayıf ya da tehdit altındayken asker alımı altını artı bu miktara kadar yardım alır.
- `AICTactics_AllyGoldSharePercent` (0–100): ayrılan altının üstündeki altından bir mal hediyesine eklenen pay; 0 hiç eklemez, istenen altın yine gönderilir.
- `AICTactics_AllyMonthlyGivePercent` (0–100): her maldan ayda verdiği pay.

Belirtilmeyen değerler modül ayarlarını kullanır (kapalı, %75, 0 ay, kapalı, kapalı, kapalı, 1000, 300, %25, %30). Monsterfish'in Ally Support'una dayanır.

### Baskınlar

- `RaidTargetPolicy`: `Native` önceki baskınları korur; `NearestReachable` yakın erişilebilir binaları seçer; `Opportunistic` tercih ve tehlikeyi değerlendirir; `RandomNearby` erişilebilir bir binayı rastgele seçip yakınındakileri temizler.
- `RaidGroupCount` (1–4) mevcut baskın gücünü böler; `RaidMinGroupSize` (1–256) küçük grupların beklemesini veya birleşmesini sağlar.
- `RaidFocus`, `Any`, `Food`, `Industry`, `HighValue` veya `{ "Food": 60, "Industry": 20 }` gibi yüzdeleri kabul eder; kalan pay her binaya açıktır. `Opportunistic` ve `RandomNearby` ile çalışır. `RaidRiskTolerance` riski, `RaidEnemyScope` hedef düşmanı belirler.

`RaidTargetPolicy` sonrasındaki beş alan için yeni baskın politikası gerekir. `RaidUnitsBase`, `RaidUnitsRandom`, `RaidUnit1..8` ve `RaidRetargetDelay` geçerlidir.

### Kuşatma ve mühendisler

- `SafeSiegePlacement` dolu inşaat yerlerinden kaçınır (modül varsayılanı KAPALI).
- `ActualSiegeResourcePayment` malzeme ve altın ister; eksikleri yapay zekâ olağan ticaretle alır (KAPALI).
- `CoordinatedSiegeHarassment` toplanan araçları erişilebilir atış konumlarına yollar (KAPALI). `SiegeHarassMinEngines` istenen en az araç sayısıdır (0–20, varsayılan 3); `HarassingSiegeEnginesMax` toplam sınır olarak kalır.
- `LargerSiegeForces`, canlı ve boş saldırı mühendisleri ile geçerli yerler kaldıkça ayarlı araçları üretir (KAPALI). `SiegeForceMax` isteğe bağlı üst sınırdır (0–64); 0'da sayı mühendislerle belirlenir. `AttMaxEngineers` mürettebatı sınırlar.
- `AICTactics_SiegeTowerCrew`, kuşatma savunma birliklerini surlara ilerleyen kuşatma kulelerine bindirir; Improved Siegeengines'in tırmanılabilir kuleleri gerekir (KAPALI).

Gelişmiş kuşatma ayarları güvenli yerleşimi otomatik açar. Mühendis rol sayımı varsayılan olarak kapalıdır; Fixed Engineers mürettebat ve araçların yaşam döngüsünü yönetir.

AIC'de belirtilen kuşatma ayarları modül varsayılanlarından önceliklidir.
