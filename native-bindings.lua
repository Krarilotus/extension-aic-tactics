local M = {}
local names = {'gameTick','rngState','rngValue','rngNext','initialDefenseTicks','aicRecords',
    'units','unitRecords','unitCapacity','tribes','tribeStride','tribeMemberWords',
    'tribeStance','tribeTargetBuilding','tribeTargetBuildingUID','buildings','buildingCapacity',
    'players','createTribe','addUnitToTribe','tribePath','entities','entityCapacity','teams',
    'assignMoatDigger','wallDefense','patrolDefense','assignRaider','assignAttacker',
    'findSortieGroup','findAttackGroup','returnTribe','removeUnitFromTribe','relayRaidOrder','mapRows','attackGroupSlots','recruitUpdate','rangedSortieNative','meleeSortieNative',
    'recruitEuropean','recruitNonEuropean','scenarioMode','scenarioCustom','scenarioMission','moat','moatVacancies','findRecruitmentBuilding','attackRecruitType','raidMaximum',
    'defenseTypes','specialDefenders','defenseSlots','raidTypes','equipmentRecipes',
    'selectAttackTarget','computeNervousness','updateAIPlayerState','returnAttack','hasNoTroopsOrAllDiggers','updateRaids','combatValue','troopValues','marketPrice','gameState',
    'originalSiegeTileCheck','siegeTileOccupancyOffset','siegeTileMap','siegeTentPointX',
    'siegeTribeIndexOffset','siegePlacementFail','siegeResourceCheck','siegeGroupMove',
    'siegePlaceTent','popUnitFromTribe','siegeAssaultBatch','siegeUnitCount',
    'siegeGetUnitForIndex','siegeCatapultRange','siegeFireRange',
    'attackPatrolDispatch','attackPatrolDestination','troopValueState','playableTiles',
    'areaMap','areaRoute','areaPathFinder','tileLogic','tileWallOwner',
    'distanceLayers','distanceLayerIndex','tribeRallyPoints','tribeRallyCount'}
-- Framework loads every module before enabling Legacy's native patches.
-- Resolve identifying contexts here; hook preflights still check current bytes
-- when a feature is activated after all dependencies have enabled.
function M.resolve()
  local game = require('config.grace').resolveNative()
  assert(type(modules.aicloader.getNativeAICLayout) == 'function',
    'AIC Tactics requires AIC Loader 1.1.4 native storage metadata')
  local layout = modules.aicloader:getNativeAICLayout()
  assert(layout.version == 1 and layout.characters == 16 and layout.stride == 676
      and type(layout.address) == 'number' and layout.address > 0,
    'AIC Tactics: unsupported AIC Loader native layout')
  game.aicRecords = layout.address
  for key, value in pairs(require('native-layout').resolve()) do game[key] = value end
  for key, value in pairs(require('native-group-actions').resolve(game)) do game[key] = value end
  for key, value in pairs(require('native-recruitment').resolve(game)) do game[key] = value end
  for key, value in pairs(require('native-aic-queries').resolve(game)) do game[key] = value end
  -- Bindings used only by the listed features; when they do not resolve, those features stay off.
  game.missingFeatures = {}
  for _, binding in ipairs({
      {'native-combat-bindings', 'Combat', 'Raids', 'Targets'},
      {'native-siege-bindings', 'SafePlacement', 'AttackPatrols', 'SiegeScreen', 'LordHunt', 'AllyRelief'},
      {'native-role-bindings', 'EngineerRoles'},
      {'native-patrol-bindings', 'AttackPatrols', 'SiegeScreen'},
      {'native-lord-hunt-bindings', 'LordHunt', 'AllyGates'},
      {'native-ally-bindings', 'AllyRelief', 'AllyGoods', 'AllyGates'},
      {'native-gate-bindings', 'AllyGates'}}) do
    local ok, result = pcall(require(binding[1]).resolve, game)
    if ok then
      for key, value in pairs(result) do game[key] = value end
    else
      for index = 2, #binding do game.missingFeatures[binding[index]] = tostring(result) end
    end
  end
  return game
end

function M.initialize(native, game)
  assert(native.nativeBindingsSize == #names * 4, 'AIC Tactics: incompatible native binding ABI')
  game = game or M.resolve()
  local allies = require('native-ally-bindings').names
  assert(native.allyBindingsSize == #allies * 4, 'AIC Tactics: incompatible ally binding ABI')
  for index, name in ipairs(names) do
    core.writeInteger(native.nativeBindings + (index - 1) * 4, game[name] or 0)
  end
  for index, name in ipairs(allies) do
    core.writeInteger(native.allyBindings + (index - 1) * 4, game[name] or 0)
  end
  native.game = game
end
return M
