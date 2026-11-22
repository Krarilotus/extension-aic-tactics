local context = require('native-context')
local M = {}

-- sendAITribe_0xf_toAttackPatrol: validates the AI, slot 15 and its UID and
-- leader, marks the tribe, then jumps to the native destination routine.
M.dispatcherSignature = '8B 4C 24 04 8B C1 69 C0 F4 39 00 00 83 B8 ? ? ? ? 00 74 52 8B D1 69 D2 F4 39 00 00 '..
  '0F BF 92 ? ? ? ? 85 D2 74 3F 8B C2 69 C9 F4 39 00 00 69 C0 ? ? ? ? 56 8B B0 ? ? ? ? '..
  '3B B1 ? ? ? ? 5E 75 21 66 83 B8 ? ? ? ? 00 74 17 66 C7 80 ? ? ? ? 01 00 89 54 24 04 B9 ? ? ? ? E9'
-- sendAttackingPatrolTribeToComputedDestination(tribe): leader-based search.
M.destinationSignature = '53 8B 5C 24 08 8B C3 69 C0 ? ? ? ? 56 0F BF B0 ? ? ? ? 57 6A 01 B9 ? ? ? ? E8 ? ? ? ? '..
  '69 F6 ? ? ? ? 0F BF 8E ? ? ? ? 0F BF 96 ? ? ? ? 0F BF 86 ? ? ? ? 6A 01 6A 0C'
-- giveTribeMoveInstruction first rejects coordinates outside the playable
-- map: playable[y * 400 + x] == 0. The same 400x400 byte map gates stations.
local playableSignature = '8B C5 69 C0 90 01 00 00 80 BC 38 ? ? ? ? 00'
-- addUnitToSmallestBehaviourTypeTribe reads AttUnitPatrolGroupsCount here.
M.patrolCountOffset = 0x65
M.patrolCountSignature = '8B 94 0A 64 02 00 00'
-- The native callers pass the player and the AI state object.
local callerPrefix = {0x56, 0x8B, 0xCB}
local scanLength = 0xA00

local function resolve(game)
  local read = core.readInteger
  local site = context.find('attack patrol dispatch', M.dispatcherSignature)
  assert(read(site + 0x0E) == game.players + 0x2300
      and read(site + 0x20) == game.players + 0x310C + 15 * 2
      and read(site + 0x32) == game.tribeStride
      and read(site + 0x39) == game.tribes + 0x34
      and read(site + 0x3F) == game.players + 0x329C + 15 * 4
      and read(site + 0x49) == game.tribes + 0x5A,
    'AIC Tactics: inconsistent native attack patrol dispatch')
  -- The dispatcher sets the patrol tribe's stance; reuse the stance binding.
  assert(read(site + 0x53) - game.tribes == game.tribeStance,
    'AIC Tactics: inconsistent native attack patrol stance')
  local troopValueState = read(site + 0x5E)
  local destination = site + 0x67 + read(site + 0x63)
  context.verify(destination, 'attack patrol destination', M.destinationSignature)
  assert(read(destination + 9) == game.tribeStride
      and read(destination + 17) == game.tribes + 0x5A
      and read(destination + 36) == 0x490
      and read(destination + 43) == game.unitRecords + 0x96
      and read(destination + 50) == game.unitRecords + 0xC6
      and read(destination + 57) == game.unitRecords + 0xC4
      and troopValueState > 0,
    'AIC Tactics: inconsistent native attack patrol destination')
  local move = game.siegeGroupMove
  context.verify(move + 0x5B, 'group move playable map check', playableSignature)
  local playable = read(move + 0x66)
  assert(playable > 0 and read(move + 0xC1) == game.mapRows,
    'AIC Tactics: inconsistent native group move map layout')
  -- Every native caller lies inside the AI state update. Require all three.
  local calls = {}
  local owner = game.updateAIPlayerState
  for offset = 3, scanLength - 5 do
    local address = owner + offset
    if core.readByte(address) == 0xE8 and address + 5 + read(address + 1) == site then
      for index, byte in ipairs(callerPrefix) do
        assert(core.readByte(address - 4 + index) == byte,
          'AIC Tactics: unexpected native attack patrol caller')
      end
      calls[#calls + 1] = address
    end
  end
  assert(#calls == 3, 'AIC Tactics: native attack patrol callers changed')
  return {attackPatrolDispatch = site, attackPatrolDestination = destination,
    troopValueState = troopValueState, playableTiles = playable, attackPatrolCalls = calls}
end

local function reason(text)
  return (tostring(text):gsub('^.-AIC Tactics: ', ''))
end

-- Checked when a character opts in (AIC files are applied after every module
-- has enabled). `expected` is the current call target of the native callers:
-- the game's dispatcher, or the module handler once installed.
function M.preflight(game, expected)
  assert(game.attackPatrolDispatch ~= 0, 'AIC Tactics: PatrolGroupOrders is unavailable: '
    .. reason(game.attackPatrolUnavailable) .. '. If another module changes the AI\'s attack patrol '
    .. 'orders (for example Smarter Patrols), turn off its patrol feature; otherwise this game '
    .. 'version is not supported for patrol orders.')
  local ok = pcall(context.verify, game.attackPatrolDispatch, 'attack patrol dispatch', M.dispatcherSignature)
  ok = ok and pcall(context.verify, game.attackPatrolDestination, 'attack patrol destination',
    M.destinationSignature)
  ok = ok and pcall(context.verify, game.findAttackGroup + M.patrolCountOffset,
    'attack patrol group count', M.patrolCountSignature)
  assert(ok, 'AIC Tactics: PatrolGroupOrders is unavailable because another module changed the '
    .. 'AI\'s attack patrol orders after AIC Tactics loaded (for example Smarter Patrols). Turn off '
    .. 'that module\'s patrol feature or remove PatrolGroupOrders from this AIC.')
  for _, site in ipairs(game.attackPatrolCalls) do
    assert(context.call(site, 'attack patrol order') == expected,
      'AIC Tactics: native attack patrol orders were replaced; restart with compatible modules')
  end
end

-- Redirect the three native callers to the module handler.
function M.install(game, handler)
  for _, site in ipairs(game.attackPatrolCalls) do core.writeCode(site, {0xE8, handler - site - 5}) end
end

-- Another module may already own the dispatcher (for example Smarter Patrols
-- replaces its entry). AIC Tactics still starts; only characters that opt in
-- to patrol orders are refused, with the reason.
function M.resolve(game)
  local ok, result = pcall(resolve, game)
  if ok then return result end
  return {attackPatrolDispatch = 0, attackPatrolDestination = 0, troopValueState = 0,
    playableTiles = 0, attackPatrolCalls = {}, attackPatrolUnavailable = tostring(result)}
end

return M
