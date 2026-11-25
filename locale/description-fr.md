AIC Tactics règle le recrutement, les attaques, les raids et les engins de siège de chaque IA installée. Ajoutez ces champs à son AIC. Les nouvelles politiques de recrutement, d’attaque et de raid sont facultatives ; les champs de siège omis suivent les options du module. Les listes et limites de troupes restent valables.

### Recrutement

- `RecruitProbSortieDefault`, `RecruitProbSortieWeak`, `RecruitProbSortieStrong` : part des sorties par niveau de force. Définir une de ces valeurs active les quatre parts pour cette IA. Défense, raid, attaque et sortie doivent totaliser 100 à chaque niveau.
- `RecruitConditions` : autres parts selon la situation. Exemple : si le château est menacé, 70 % de défense, 20 % d’attaque et 10 % de sorties. La première règle applicable vaut pour ce recrutement.
- `RecruitPolicy` : choix de compatibilité facultatif. `Native` conserve la sélection d’origine ; `WeightedRoles` active explicitement les quatre parts.
- `DefRecruitComposition` : `PreserveSlots` respecte les proportions de `DefUnit1..8` ; `Native` garde la sélection habituelle.
- `RecruitInitialDefenseMonths` : durée pendant laquelle raids et armée principale attendent que la défense atteigne son quota. 0 supprime l’attente ; sans valeur, la minuterie initiale du jeu s’applique (6 mois, ou celle de Legacy `ai_recruitstate_initialtimer`).

Une part de sortie active `WeightedRoles` automatiquement. Les règles de situation, la composition défensive et `RecruitInitialDefenseMonths` exigent de le définir explicitement. Intervalles et quotas de recrutement restent applicables.

### Attaques et riposte

- `AttackTargetPolicy` : `Inherit` utilise `TargetChoice`. `LowestPopulation`, `FewestTroops`, `LowestCombatPower`, `Random` et `LastAggressor` choisissent une cible par attaque. Pour la garder entre les attaques : `{ "Choice": "LastAggressor", "UntilDefeated": true }`. Vous pouvez ajouter `"Provocation": { "LossPower": 200 }` pour régler les représailles.
- `AttackTargetCommitment` : reste accepté pour les anciens AIC. Dans un nouvel AIC, choisissez la cible et sa durée dans `AttackTargetPolicy`.
- `AttackPreparation` : `DuringAttack` prépare la prochaine vague au château pendant l’attaque ; `Native` garde le calendrier habituel.
- `AttackActivation` : `AfterProvocation` attend une attaque ennemie suffisante avant de lancer armées et raids ; `Immediate` n’attend pas. Les sorties défensives restent possibles.
- `ProvocationRules` : reste accepté pour les anciens AIC, `AfterProvocation` et la menace utilisée au recrutement. Pour `LastAggressor`, un nouvel AIC peut placer les valeurs dans `AttackTargetPolicy.Provocation`.

### Patrouilles d’attaque

- `AICTactics_PatrolOrders` : `Native` ne donne d’ordres qu’au premier groupe de patrouille d’attaque ; le second attend à son point de rassemblement. `AllGroups` envoie aussi le second groupe lors d’un assaut. Il lui faut `AttUnitPatrolGroupsCount` 2 pour disposer de troupes.
- `AICTactics_PatrolFormation` : destination du second groupe. `Native` le laisse choisir ses propres déplacements ; `Wing` le garde à côté du premier groupe ; `Mirror` l’envoie de l’autre côté du donjon ciblé. Exige `AllGroups`.
- `AICTactics_PatrolSpacing` (4–40, défaut 12) : cases entre les deux groupes en `Wing`.

Les réglages de patrouille ne conservent aucun état sauvegardé : une sauvegarde se recharge avec les deux groupes toujours dans leurs emplacements d’origine. `AttUnitPatrol` et `AttUnitPatrolMax` restent valables.

### Raids

- `RaidTargetPolicy` : `Native` garde les raids habituels ; `NearestReachable` vise des bâtiments proches accessibles ; `Opportunistic` tient compte des préférences et du danger ; `RandomNearby` choisit une cible accessible au hasard, puis nettoie les bâtiments proches.
- `RaidGroupCount` (1–4) partage la force de raid existante ; `RaidMinGroupSize` (1–256) fait attendre ou réunir les petits groupes.
- `RaidFocus` accepte `Any`, `Food`, `Industry`, `HighValue` ou des pourcentages comme `{ "Food": 60, "Industry": 20 }` ; le reste vise tout bâtiment. Fonctionne avec `Opportunistic` et `RandomNearby`. `RaidRiskTolerance` règle le risque ; `RaidEnemyScope` choisit l’ennemi principal ou tout ennemi.

Les cinq champs après `RaidTargetPolicy` exigent une nouvelle politique de raid. `RaidUnitsBase`, `RaidUnitsRandom`, `RaidUnit1..8` et `RaidRetargetDelay` restent valables.

### Siège et ingénieurs

- `SafeSiegePlacement` évite les emplacements occupés (module : ACTIVÉ).
- `ActualSiegeResourcePayment` exige matériaux et or ; l’IA achète les manques par son commerce normal (DÉSACTIVÉ).
- `CoordinatedSiegeHarassment` envoie des engins rassemblés vers des positions de tir accessibles (DÉSACTIVÉ). `SiegeHarassMinEngines` fixe le minimum souhaité (0–20, défaut 3) ; `HarassingSiegeEnginesMax` reste la limite totale.
- `LargerSiegeForces` répète la composition tant que des ingénieurs d’attaque vivants et libres et des sites valides restent disponibles (DÉSACTIVÉ). `SiegeForceMax` est un plafond facultatif (0–64) ; 0 laisse décider le nombre d’ingénieurs. `AttMaxEngineers` limite les équipages.

Le placement sûr s’active avec les options de siège avancées. Le décompte des rôles d’ingénieur est activé par défaut ; Fixed Engineers gère le cycle de vie des équipages.

Les valeurs de siège définies dans l’AIC priment sur celles du module.
