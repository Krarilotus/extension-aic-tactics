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

- `AICTactics_PatrolOrders` : `AllGroups` envoie aussi le second groupe de patrouille d’attaque lors d’un assaut ; `Native` ne commande que le premier. Il lui faut `AttUnitPatrolGroupsCount` 2 pour disposer de troupes.
- `AICTactics_PatrolFormation` : `Wing` garde les autres groupes en ligne à côté du premier ; `Mirror` les envoie de l’autre côté du donjon ciblé ; `Routes` tourne autour du château ciblé dans les deux sens et parcourt son front en va-et-vient, comme Smarter Patrols ; `Native` les laisse choisir leurs propres déplacements.
- `AICTactics_PatrolSpacing` (4–40) : cases entre groupes voisins en `Wing` et `Mirror`.
- `AICTactics_PatrolExtraGroups` (0–6) : groupes de patrouille en plus pendant une attaque ; ils se regroupent à la fin.
- `AICTactics_PatrolMinSize` (1–50) : unités que garde chaque groupe quand la patrouille est divisée.
- `AICTactics_PatrolRadius` (4–100) : distance de marche entre le château ciblé et l’anneau que suit `Routes`.
- `AICTactics_SiegeScreen` : `EngineApproach` place les groupes de défense de siège devant leurs engins, les deux groupes à des engins différents ; `Native` garde le ralliement du jeu.
- `AICTactics_SiegeScreenForward` (0–10) : cases devant les engins pour les groupes de mêlée.
- `AICTactics_SiegeScreenExtraGroups` (0–6) : groupes de défense de siège en plus pendant que les engins travaillent, chacun à son engin ; ils se regroupent avant l’assaut des murs.

Les valeurs omises suivent les réglages du module (premier groupe seulement, `Native`, 12 cases, aucun groupe en plus, groupes de 4, anneau 12, escorte désactivée, 4 cases, aucun groupe d’escorte en plus).

### Chasse au seigneur

Quand une IA attaque un joueur humain dont le seigneur s’abrite derrière les murs d’un allié, elle assiège ce château au lieu du foyer vide. Dès que la voie vers le seigneur est libre, une partie de l’armée fonce sur lui.

- `AICTactics_LordHuntRetarget` : `true` attaque le château où se cache le seigneur ; `false` garde l’attaque sur son propre château.
- `AICTactics_LordHuntSiege` : `HidingCastle` ou `HomeCastle`, le château qui reçoit le siège complet tant que celui du seigneur a encore habitants et bâtiments. L’autre château reçoit une part des troupes.
- `AICTactics_LordHuntSplitShare` (0–100) : cette part en pourcentage.
- `AICTactics_LordHuntSwitchPercent` (0–100) : pendant une attaque, l’IA ne change de château que s’il lui reste cette part de son armée.
- `AICTactics_LordHuntShare` (0–100) : part de l’armée au château du seigneur qui le traque ; 0 désactive la chasse.

Les valeurs omises suivent les réglages du module (désactivé, `HidingCastle`, 50, 80, 0). Les attaques d’IA contre des joueurs IA ne changent pas.

### Aide aux alliés

Les alliés IA s’entraident et vous aident, pas seulement sur demande.

- `AICTactics_AllyRelief` : `true` envoie une partie de l’armée inactive à un allié dont le château est attaqué, et quand vous demandez de l’aide ; `false` garde l’aide du jeu.
- `AICTactics_AllyArmySharePercent` (0–100) : part de l’armée d’attaque inactive envoyée ; 0 n’envoie personne, même sur demande.
- `AICTactics_AllyReliefMinMonths` (0–12) : nombre minimal de mois que reste le secours une fois l’allié en paix.
- `AICTactics_AllyReliefGates` : `true` ouvre au secours les portes fermées de l’allié, à l’arrivée comme au départ.
- `AICTactics_AllyReliefKeep` : `true` place le secours sur le donjon de l’allié plutôt qu’à son entrée.
- `AICTactics_AllyGoods` : `true` permet à l’IA de demander biens et or à ses alliés IA, de leur en donner et de vous envoyer de l’or sur demande.
- `AICTactics_AllyGoldKeep` (0–100000) : or gardé avant tout don, même sur votre demande.
- `AICTactics_AllyRecruitGoldBuffer` (0–5000) : faible ou menacée, elle est aidée jusqu’à son or de recrutement plus ce montant.
- `AICTactics_AllyGoldSharePercent` (0–100) : part de son or au-delà de la réserve jointe à un don de biens ; 0 n’ajoute rien, l’or demandé reste envoyé.
- `AICTactics_AllyMonthlyGivePercent` (0–100) : part de chaque bien donnée par mois.

Les valeurs omises suivent les réglages du module (désactivé, 75 %, 0 mois, désactivé, désactivé, désactivé, 1000, 300, 25 %, 30 %). D’après Ally Support de Monsterfish.

### Raids

- `RaidTargetPolicy` : `Native` garde les raids habituels ; `NearestReachable` vise des bâtiments proches accessibles ; `Opportunistic` tient compte des préférences et du danger ; `RandomNearby` choisit une cible accessible au hasard, puis nettoie les bâtiments proches.
- `RaidGroupCount` (1–4) partage la force de raid existante ; `RaidMinGroupSize` (1–256) fait attendre ou réunir les petits groupes.
- `RaidFocus` accepte `Any`, `Food`, `Industry`, `HighValue` ou des pourcentages comme `{ "Food": 60, "Industry": 20 }` ; le reste vise tout bâtiment. Fonctionne avec `Opportunistic` et `RandomNearby`. `RaidRiskTolerance` règle le risque ; `RaidEnemyScope` choisit l’ennemi principal ou tout ennemi.

Les cinq champs après `RaidTargetPolicy` exigent une nouvelle politique de raid. `RaidUnitsBase`, `RaidUnitsRandom`, `RaidUnit1..8` et `RaidRetargetDelay` restent valables.

### Siège et ingénieurs

- `SafeSiegePlacement` évite les emplacements occupés (module : DÉSACTIVÉ).
- `ActualSiegeResourcePayment` exige matériaux et or ; l’IA achète les manques par son commerce normal (DÉSACTIVÉ).
- `CoordinatedSiegeHarassment` envoie des engins rassemblés vers des positions de tir accessibles (DÉSACTIVÉ). `SiegeHarassMinEngines` fixe le minimum souhaité (0–20, défaut 3) ; `HarassingSiegeEnginesMax` reste la limite totale.
- `LargerSiegeForces` répète la composition tant que des ingénieurs d’attaque vivants et libres et des sites valides restent disponibles (DÉSACTIVÉ). `SiegeForceMax` est un plafond facultatif (0–64) ; 0 laisse décider le nombre d’ingénieurs. `AttMaxEngineers` limite les équipages.
- `AICTactics_SiegeTowerCrew` fait monter les troupes de défense de siège dans les tours de siège en route vers les murs ; exige des tours escaladables d’Improved Siegeengines (DÉSACTIVÉ).

Le placement sûr s’active avec les options de siège avancées. Le décompte des rôles d’ingénieur est désactivé par défaut ; Fixed Engineers gère le cycle de vie des équipages.

Les valeurs de siège définies dans l’AIC priment sur celles du module.
