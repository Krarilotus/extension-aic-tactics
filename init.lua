local nativeGame = require('native-bindings').resolve()
local enabled = false
return {
  enable = function(self, config)
    assert(not enabled, 'AIC Tactics requires a clean process')
    assert(type(modules.aicloader.registerAICUpdateProvider) == 'function',
      'AIC Tactics requires the transactional AIC Loader provider API')
    local mapState = assert(modules['map-extensions'], 'AIC Tactics requires Map Extensions')
    assert(type(mapState.requiredStateVersion) == 'function' and mapState:requiredStateVersion() == 1,
      'AIC Tactics requires Map Extensions 1.1.0 with required state admission')
    local saveInterface = type(mapState.getNativeSaveInterface) == 'function'
      and mapState:getNativeSaveInterface()
    assert(saveInterface and saveInterface.failureHandling == 1 and saveInterface.readContext == 1,
      'AIC Tactics requires Map Extensions 1.1.4 for native load context and rejected state handling')
    local native = require('native').new(nativeGame)
    local safePlacement = config and config.safeSiegePlacement
    if safePlacement == nil then safePlacement = false end
    local siegePayment = config and config.actualSiegeResourcePayment
    if siegePayment == nil then siegePayment = false end
    local siegeHarassment = config and config.coordinatedSiegeHarassment
    if siegeHarassment == nil then siegeHarassment = false end
    local siegeHarassMinimum = config and config.siegeHarassMinEngines
    if siegeHarassMinimum == nil then siegeHarassMinimum = 3 end
    local largerSiege = config and config.largerSiegeForces
    if largerSiege == nil then largerSiege = false end
    local siegeForceMaximum = config and config.siegeForceMax
    if siegeForceMaximum == nil then siegeForceMaximum = 0 end
    -- The other siege features need safe placement, so it is configured first.
    if siegePayment or siegeHarassment or largerSiege then safePlacement = true end
    native.configureSafePlacement(safePlacement)
    native.configureSiegePayment(siegePayment)
    native.configureSiegeHarassment(siegeHarassment, siegeHarassMinimum)
    -- A module enabled after this one may still change the siege ranges.
    hooks.registerHookCallback('afterInit', native.refreshSiegeRanges)
    native.configureLargerSiegeForce(largerSiege, siegeForceMaximum)
    local roleCounting = config and config.correctEngineerRoleCounting
    if roleCounting == nil then roleCounting = false end
    native.configureEngineerRoles(roleCounting)
    native.configureAttackPatrols(require('config.attacks').fallbacks(config))
    native.configureLordHunt(require('config.lord-hunt').fallbacks(config))
    native.configureAllies(require('config.allies').fallbacks(config))
    local diagnostics = config and config.diagnostics
    assert(diagnostics == nil or type(diagnostics) == 'boolean', 'AIC Tactics: diagnostics must be boolean')
    core.writeInteger(native.diagnostics, diagnostics and 1 or 0)
    local protocol=assert(modules.protocol,'AIC Tactics requires Protocol 1.1.10')
    assert(type(protocol.multiplayerAdmissionVersion)=='function' and protocol:multiplayerAdmissionVersion()==1,
      'AIC Tactics requires Protocol multiplayer admission')
    local backend=require('config.backend').new(native)
    require('config.provider').register(modules.aicloader, backend)
    require('siege-tower-crew').register(modules['improved-siegeengines'], native, nativeGame,
      config and config.siegeTowerCrew == true)
    -- Legacy's target choice and recruitment interval feed AIC decisions, so
    -- the match identity includes them.
    core.writeInteger(native.legacyTargetPolicy, native.legacyTargetChoice())
    local version
    for _, extension in ipairs(allActiveExtensions) do
      if extension.name == 'aic-tactics' then version = extension.version end
    end
    self.recruitmentState = require('state').new(native, native.intervalForced(), version)
    protocol:registerMultiplayerAdmission('aic-tactics',function()
      backend.freezeMultiplayer()
      return sha.sha256(self.recruitmentState.matchIdentity())
    end,function(reason)
      -- The existing chat owner displays locally; do not broadcast another
      -- admission message or submit a simulation command for presentation.
      modules.chat:fireChatEvent(reason=='mismatch'
        and 'AIC Tactics: settings differ. Use the same extension versions and AI settings.'
        or reason=='error' and 'AIC Tactics: settings could not be verified. Check the UCP log and restart the game.'
        or 'AIC Tactics: checking settings. Press Start again when all players have responded.',0,0)
    end)
    -- The owners' answers are fixed for a match, so they are read when its map or save is read.
    local callbacks = self.recruitmentState.callbacks
    for _, name in ipairs({'initialize', 'deserialize'}) do
      local read = callbacks[name]
      callbacks[name] = function(...)
        native.refreshOwnerAnswers()
        return read(...)
      end
    end
    mapState:registerSection('aic-tactics', callbacks,
      {required=true, format='aic-tactics-state-14'})
    log(INFO, string.format('[aic-tactics] configuration=0x%X size=%d observations=0x%X size=%d',
      native.configuration, native.configurationSize, native.observations, native.observationSize))
    local addresses = {}
    for _, name in ipairs({'combatCensus','combatCensusTick','combatCensusValid','incidents',
        'targetStates','targetLifecycle','reserves','raidStates','raidGroupCensus',
        'raidUnitPower','raidStaticDefenses','raidBuildingCensusTick','raidBuildingCensusValid'}) do
      addresses[#addresses + 1] = string.format('%s=0x%X', name, native[name])
    end
    log(INFO, '[aic-tactics] state-abi=7 ' .. table.concat(addresses, ' '))
    enabled = true
  end,
  disable = function() error('AIC Tactics requires a game restart to disable') end,
}
